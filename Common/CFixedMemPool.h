/*
- 고정 블록 풀: 미리 큰 슬랩(slab)을 잡아 같은 크기의 블록을 다수로 쪼개서 제공
- thread_local 캐시: 각 스레드가 자기 캐시에 있으면 락 없이 빠르게 할당/반납
- 중앙 풀: 캐시가 바닥나면 중앙 풀에서 배치로 가져오고, 캐시가 너무 많아지면 중앙으로 되돌림
- 안전성: 중앙 풀 접근만 std::mutex, 나머지는 락 프리(스레드 로컬)
*/

#pragma once
#include <cstddef>
#include <cstdint>
#include <vector>
#include <mutex>
#include <memory>
#include <new>
#include <cassert>
#include <algorithm>
#include <atomic>

#ifndef CACHELINE_SIZE
#define CACHELINE_SIZE 64
#endif


//“n을 a의 배수로 올림(align up)”
//즉, n보다 크거나 같은 가장 가까운 a의 배수로 맞춥니다.
//단, a 는 2의 배수이어야 함
//ex) n:13, a:8 ==> return 16 (8의 배수중 13 이상 첫번째)
//    n:64, a:8 ==> return 64
static inline std::size_t align_up(std::size_t n, std::size_t a) {
    return (n + (a - 1)) & ~(a - 1);
}

class CFixedBlockPool 
{
public:
    // blockSize: 사용할 블록 크기(바이트)
    // blocksPerSlab: 슬랩 하나에 몇 블록씩 만들지(배치 크기)
    // tlCapacity: thread_local 캐시에 유지할 최대 블록 수
    CFixedBlockPool(std::size_t blockSize,
        std::size_t blocksPerSlab = 4096,
        std::size_t tlCapacity = 256)
        : m_blockSize(align_up(blockSize, alignof(std::max_align_t))),
        m_blocksPerSlab(blocksPerSlab),
        m_tlCapacity(tlCapacity)
    {
        assert(m_blockSize >= sizeof(void*) && "blockSize too small");
    }

    ~CFixedBlockPool() {
        // 모든 슬랩 해제
        for (auto p : m_slabs) ::operator delete(p, std::align_val_t(CACHELINE_SIZE));
    }


    // 템플릿 객체 생성/소멸 보조 (고정크기 내에서만)
    template<typename T, typename... Args>
    T* get(Args&&... args) {
        assert(sizeof(T) <= sizeof_block() && "T too big for pool block");
        void* mem = allocate();
        return new (mem) T(std::forward<Args>(args)...);
    }

    template<typename T>
    void release(T* p) {
        if (!p) return;
        p->~T();
        deallocate(p);
    }


    //********** 이후 공부를 더 하고 나서 오픈. deleter 에 대해 **************/
    //// unique_ptr와 함께 쓰고 싶을 때: 커스텀 deleter
    //template<typename T>
    //struct releasePtr {
    //    CFixedBlockPool* pool{};
    //    void operator()(T* p) const noexcept {
    //        if (!p) return;
    //        p->~T();
    //        pool->deallocate(p);
    //    }
    //};


    //template<typename T>
    //using unique_pool_ptr = std::unique_ptr<T, releasePtr<T>>;  // 두번째 파라미터가 deleter

    //template<typename T, typename... Args>
    //unique_pool_ptr<T> make_unique(Args&&... args) {
    //    T* p = get<T>(std::forward<Args>(args) ...);
    //    return unique_pool_ptr<T>(p, releasePtr<T>{ this });
    //}


    std::size_t sizeof_block() const noexcept { return m_blockSize; }

private:

    // 블록 하나 할당
    void* allocate() 
    {
        auto& cache = local();
        if (!cache.freeList.empty()) {
            void* p = cache.freeList.back();
            cache.freeList.pop_back();
            return p;
        }
        refill_cache_from_central(cache);

        // 여전히 못 받았으면 최후 슬로우패스: 슬랩 생성 후 재시도
        if (cache.freeList.empty()) {
            allocate_new_slab();
            refill_cache_from_central(cache);
        }
        void* p = cache.freeList.back();
        cache.freeList.pop_back();
        return p;
    }

    // 블록 반납
    void deallocate(void* p) {
        if (!p) return;
        auto& cache = local();
        cache.freeList.push_back(p);
        if (cache.freeList.size() > m_tlCapacity) {
            // 넘친 만큼 중앙으로 일부 반납
            std::vector<void*> spill;
            spill.reserve(cache.freeList.size() - m_tlCapacity);
            while (cache.freeList.size() > m_tlCapacity) {
                spill.push_back(cache.freeList.back());
                cache.freeList.pop_back();
            }
            return_to_central(spill);
        }
    }


private:
    struct alignas(CACHELINE_SIZE) CentralFreeList {
        std::vector<void*> nodes; // 중앙 자유 리스트
    };

    // 스레드 로컬 캐시
    struct ThreadCache {
        std::vector<void*> freeList;
    };

    ThreadCache& local() {
        thread_local ThreadCache tc;
        return tc;
    }

    void allocate_new_slab() {
        // 슬랩 하나를 통째로 잡고, 블록으로 쪼개 중앙 리스트에 넣음
        const std::size_t slabBytes = m_blockSize * m_blocksPerSlab;
        void* slab = ::operator new(slabBytes, std::align_val_t(CACHELINE_SIZE));
        {
            std::lock_guard<std::mutex> lk(m_mu);
            m_slabs.push_back(slab);
            char* base = static_cast<char*>(slab);
            for (std::size_t i = 0; i < m_blocksPerSlab; ++i) {
                m_central.nodes.push_back(base + i * m_blockSize);
            }
        }
    }

    void refill_cache_from_central(ThreadCache& cache) {
        std::lock_guard<std::mutex> lk(m_mu);
        // 중앙에 없으면 바로 리턴
        if (m_central.nodes.empty()) return;

        // 한번에 여러 개를 캐시에 채움 (최대 tlCapacity/2)
        const std::size_t want = min(m_tlCapacity / 2 + 1, m_central.nodes.size());
        cache.freeList.reserve(cache.freeList.size() + want);
        for (std::size_t i = 0; i < want; ++i) {
            cache.freeList.push_back(m_central.nodes.back());
            m_central.nodes.pop_back();
        }
    }

    void return_to_central(const std::vector<void*>& spill) {
        std::lock_guard<std::mutex> lk(m_mu);
        for (void* p : spill) m_central.nodes.push_back(p);
    }

private:
    const std::size_t m_blockSize;
    const std::size_t m_blocksPerSlab;
    const std::size_t m_tlCapacity;

    CentralFreeList m_central;
    std::mutex m_mu;
    std::vector<void*> m_slabs;
};




/*
************* 사용법 *****************

CFixedBlockPool msgPool(sizeof(MyMessage));

// producer-A/B
auto msg = msgPool.make_unique<MyMessage>(args...);
queue.push(std::move(msg));        // 소유권 이동 (raw 포인터 아님)

or
auto* p = pool.get<Packet>();

// consumer
auto msg_a = queue.pop();          // unique_ptr 획득
process(*msg_a);                   // 사용
// 스코프 종료 시 자동으로 destroy 호출 → 캐시/중앙 풀로 반납
*/