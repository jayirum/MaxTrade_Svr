#pragma once

#include <algorithm>

constexpr size_t CAP_SIZE = 4096;                // SPSC ť �뷮(2�� �ŵ����� ����)


template <typename T>
class CspscRing 
{
public:
    bool push(const T& v)
    {
        auto h = m_head.load(std::memory_order_relaxed);    // �ܼ��� ���ڼ��� ����, ���� ���� ����. �����߿�
        auto next = get_next(h);
        if (next == m_tail.load(std::memory_order_acquire)) return false; // full. ������ �޸� ������ �� load ���Ŀ� ����ǵ��� ����. �д���

        m_buf[h] = v;

        m_head.store(next, std::memory_order_release);  // ������ �޸� ������ �� store ������ ����ǵ��� ����. ���� ��
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