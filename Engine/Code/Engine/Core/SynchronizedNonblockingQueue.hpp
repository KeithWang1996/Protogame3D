#pragma once
#include <queue>
#include <mutex>
#include <atomic>
#include <condition_variable>
template<typename T>
class SynchronizedNonblockingQueue : protected std::queue<T>
{
public:
	using value_type = typename T;
protected:
	using base = typename std::queue<T>;
public:
	SynchronizedNonblockingQueue() : base() {};
	SynchronizedNonblockingQueue(SynchronizedNonblockingQueue const&) = delete;
	SynchronizedNonblockingQueue(SynchronizedNonblockingQueue const&&) = delete;
	~SynchronizedNonblockingQueue() = default;

	SynchronizedNonblockingQueue& operator=(SynchronizedNonblockingQueue const&) = delete;
	SynchronizedNonblockingQueue& operator=(SynchronizedNonblockingQueue const&&) = delete;
	size_t GetSize();
	void push(const value_type& value);
	value_type pop();
protected:
	void lock();
	void unlock();
private:
	const int UNLOCKED = 0;
	const int LOCKED = 1;
	std::atomic<int> m_atomic;
};

template<typename T>
void SynchronizedNonblockingQueue<T>::lock()
{
	int expected = UNLOCKED;
	while (!m_atomic.compare_exchange_strong(expected, LOCKED));
}

template<typename T>
void SynchronizedNonblockingQueue<T>::unlock()
{
	int expected = LOCKED;
	while (!m_atomic.compare_exchange_strong(expected, UNLOCKED));
}

template<typename T>
void SynchronizedNonblockingQueue<T>::push(const value_type& value)
{
	lock();
	base::push(value);
	unlock();
}

template<typename T>
size_t SynchronizedNonblockingQueue<T>::GetSize()
{
	return base::size();
}

template<typename T>
typename SynchronizedNonblockingQueue<T>::value_type SynchronizedNonblockingQueue<T>::pop()
{
	value_type value = value_type();
	lock();
	if (!base::empty())
	{
		value = base::front();
		base::pop();
	}
	unlock();
	return value;
}