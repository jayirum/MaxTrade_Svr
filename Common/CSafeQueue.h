#pragma once

#include <algorithm>
#include <memory>
#include <thread>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <atomic>

//************** thread 간 데이터 전달위한 큐 **************//


template<typename T>
class CSafeQueue
{
public:
	void push(T* item) {
		{
			std::lock_guard<std::mutex> lock(m_mutex);
			m_queue.push(item);
		}
		m_cond.notify_one();
	}

	// blocking pop (큐가 비어있으면 대기)
	T* wait_and_pop() {
		std::unique_lock<std::mutex> lock(m_mutex);
		m_cond.wait(lock, [this] { return !m_queue.empty() || m_closed; });
		if (m_queue.empty())
			return nullptr;

		T* item = m_queue.front();
		m_queue.pop();
		return item;
	}

	// non-blocking pop (비어있으면 null 반환)
	T* try_pop() {
		std::lock_guard<std::mutex> lock(m_mutex);
		if (m_queue.empty())
			return nullptr;
		T* item = m_queue.front();
		m_queue.pop();
		return item;
	}

	bool empty() const {
		std::lock_guard<std::mutex> lock(m_mutex);
		return m_queue.empty();
	}

	void close() {
		{
			std::lock_guard<std::mutex>lock(m_mutex);
			m_closed = true;
		}
		m_cond.notify_all();
	}

private:
	bool m_closed{ false };
	mutable std::mutex m_mutex;
	std::queue<T*> m_queue;
	std::condition_variable m_cond;
};



template<typename T>
class CUniquePtrQueue
{
public:
	// unique_ptr을 이동(push)
	void push(std::unique_ptr<T>&& item) {
		{
			std::lock_guard<std::mutex> lock(m_mutex);
			m_queue.push(std::move(item));
		}
		m_cond.notify_one();
	}

	// blocking pop (큐가 비어있으면 대기)
	std::unique_ptr<T> wait_and_pop() {
		std::unique_lock<std::mutex> lock(m_mutex);
		m_cond.wait(lock, [this] { return !m_queue.empty() || m_closed; });
		if(m_queue.empty())
			return nullptr;

		std::unique_ptr<T> item = std::move(m_queue.front());
		m_queue.pop();
		return item;
	}

	// non-blocking pop (비어있으면 null 반환)
	std::unique_ptr<T> try_pop() {
		std::lock_guard<std::mutex> lock(m_mutex);
		if (m_queue.empty())
			return nullptr;
		std::unique_ptr<T> item = std::move(m_queue.front());
		m_queue.pop();
		return item;
	}

	bool empty() const {
		std::lock_guard<std::mutex> lock(m_mutex);
		return m_queue.empty();
	}

	void close() {
		{
			std::lock_guard<std::mutex>lock(m_mutex);
			m_closed = true;
		}
		m_cond.notify_all();
	}

private:
	bool m_closed{ false };
	mutable std::mutex m_mutex;
	std::queue<std::unique_ptr<T>> m_queue;
	std::condition_variable m_cond;
};


template<typename T>
class CSharedPtrQueue
{
public:
	// shared_ptr을 이동(push)
	void push(std::shared_ptr<T>&& item) {
		{
			std::lock_guard<std::mutex> lock(m_mutex);
			m_queue.push(std::move(item));
		}
		m_cond.notify_one();
	}

	// blocking pop (큐가 비어있으면 대기)
	std::shared_ptr<T> wait_and_pop(_In_ std::atomic_bool& is_finished) {
		std::unique_lock<std::mutex> lock(m_mutex);
		m_cond.wait(lock, [this] { return !m_queue.empty() || is_finished.load(memory_order_acquire); });
		std::shared_ptr<T> item = std::move(m_queue.front());
		m_queue.pop();
		return item;
	}

	// non-blocking pop (비어있으면 null 반환)
	std::shared_ptr<T> try_pop() {
		std::lock_guard<std::mutex> lock(m_mutex);
		if (m_queue.empty())
			return nullptr;
		std::shared_ptr<T> item = std::move(m_queue.front());
		m_queue.pop();
		return item;
	}

	bool empty() const {
		std::lock_guard<std::mutex> lock(m_mutex);
		return m_queue.empty();
	}

private:
	mutable std::mutex m_mutex;
	std::queue<std::shared_ptr<T>> m_queue;
	std::condition_variable m_cond;
};