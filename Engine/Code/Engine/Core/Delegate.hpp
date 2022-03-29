#pragma once
#include <functional>
#include <vector>
template <typename ...ARGS>
class Delegate
{
public:
	using c_callback_t = void (*)(ARGS...);            // signature of a standalone C function that works with this Delegate
	using function_t = std::function<void(ARGS...)>;   // callable object to store it
	struct sub_t {
		void const* obj_id;           // object I'm calling on
		void const* func_id;          // address of function I'm calling
		function_t callback;    // callable object o call above function/method

		inline bool operator==(sub_t const& other) const { return (obj_id == other.obj_id) && (func_id == other.func_id); }
	};
	// typedef the callback type to save on typing later
public:
	std::vector<sub_t> m_subscribers;
public:
	void subscribe(c_callback_t const& cb)
	{
		sub_t sub;
		sub.obj_id = nullptr;
		sub.func_id = (void const*)& cb;
		sub.callback = cb;
		subscribe(sub);
	}

	void unsubscribe(c_callback_t const& cb)
	{
		sub_t sub;
		sub.obj_id = nullptr;
		sub.func_id = (void const*)& cb;
		unsubscribe(sub);
	}

	template <typename OBJ_TYPE>
	void subscribe_method(OBJ_TYPE* obj, void (OBJ_TYPE::* mcb)(ARGS...))
	{
		sub_t sub;
		sub.obj_id = obj;
		sub.func_id = *(void const**)& mcb;
		sub.callback = [=](ARGS ...args) { (obj->*mcb)(args...); };
		subscribe(sub);
	}

	template <typename OBJ_TYPE>
	void unsubscribe_method(OBJ_TYPE* obj, void (OBJ_TYPE::* mcb)(ARGS...))
	{
		sub_t sub;
		sub.obj_id = obj;
		sub.func_id = *(void const**)& mcb;
		unsubscribe(sub);
	}

	void unsubscribe_all()
	{
		m_subscribers.clear();
	}

	void invoke(ARGS& ...args) const
	{
		for (const sub_t& sub : m_subscribers) {
			sub.callback(args...);
		}
	}
	void operator() (ARGS& ...args) const { return invoke(args...); }

private:
	// moving this private as no one will use these directly
	void subscribe(sub_t const& func_to_call)
	{
		m_subscribers.push_back(func_to_call);
	}

	void unsubscribe(sub_t const& sub)
	{
		for (int i = 0; i < m_subscribers.size(); ++i) {
			if (m_subscribers[i] == sub) {
				m_subscribers.erase(m_subscribers.begin() + i);
				return;
			}
		}
	}
};