#pragma once
#include <queue>
#include <mutex>
#include <condition_variable>
template<typename T>
class SynchronizedBlockingQueue : protected std::queue<T>
{
public:
	using value_type = typename T;
protected:
	using base = typename std::queue<T>;
public:
	SynchronizedBlockingQueue() : base() {};
	SynchronizedBlockingQueue(SynchronizedBlockingQueue const&) = delete;
	SynchronizedBlockingQueue(SynchronizedBlockingQueue const&&) = delete;
	~SynchronizedBlockingQueue() = default;

	SynchronizedBlockingQueue& operator=(SynchronizedBlockingQueue const&) = delete;
	SynchronizedBlockingQueue& operator=(SynchronizedBlockingQueue const&&) = delete;

	size_t GetSize();
	void push(const value_type& value);
	value_type pop();
	void notify_all();
protected:

private:
	std::mutex m_lock;
	std::condition_variable m_condition;
};

template<typename T>
void SynchronizedBlockingQueue<T>::push(const value_type& value)
{
	std::lock_guard<std::mutex> guard(m_lock);
	base::push(value);
	m_condition.notify_all();
}

template<typename T>
size_t SynchronizedBlockingQueue<T>::GetSize()
{
	return base::size();
}

template<typename T>
typename SynchronizedBlockingQueue<T>::value_type SynchronizedBlockingQueue<T>::pop()
{
	value_type value = value_type();
	std::unique_lock<std::mutex> uniqueLock(m_lock);
	if (base::empty())
	{
		m_condition.wait(uniqueLock);
	}
	//If it is still empty
	if (base::empty())
	{
		return value;
	}
	value = base::front();
	base::pop();
	return value;
}

template<typename T>
void SynchronizedBlockingQueue<T>::notify_all()
{
	m_condition.notify_all();
}