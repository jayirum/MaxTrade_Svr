#pragma once

#include <algorithm>

constexpr size_t CAP_SIZE = 4096;                // SPSC 큐 용량(2의 거듭제곱 권장)


template <typename T>
class CspscRing 
{
public:
    bool push(const T& v)
    {
        auto h = m_head.load(std::memory_order_relaxed);    // 단순히 원자성만 보장, 순서 보장 없음. 성능중요
        auto next = get_next(h);
        if (next == m_tail.load(std::memory_order_acquire)) return false; // full. 이후의 메모리 접근이 이 load 이후에 실행되도록 보장. 읽는쪽

        m_buf[h] = v;

        m_head.store(next, std::memory_order_release);  // 이전의 메모리 접근이 이 store 이전에 실행되도록 보장. 쓰는 쪽
        return true;
    }
    bool pop(T& out)
    {
        auto tail = m_tail.load(std::memory_order_relaxed);
        if (tail == m_head.load(std::memory_order_acquire)) return false; // empty

        out = std::move(m_buf[tail]);

        m_tail.store(get_next(tail), std::memory_order_release);
        return true;
    }
private:
    static_assert((CAP_SIZE& (CAP_SIZE - 1)) == 0, "CAP_SIZE must be power of two");
    static constexpr size_t get_next(size_t x) { return (x + 1) & (CAP_SIZE - 1); }

    T m_buf[CAP_SIZE]{};
    std::atomic<size_t> m_head{ 0 }, m_tail{ 0 };
};