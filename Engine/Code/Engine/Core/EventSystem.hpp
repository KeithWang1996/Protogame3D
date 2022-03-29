#pragma once
#include <string>
#include <vector>
#include "Engine/Core/Delegate.hpp"
typedef void(*EventCallbackFunctionPtrType)();
typedef void(*EventCallbackFunctionWithArgPtrType)(int);
class NamedStrings;
class NamedProperties;
typedef void(*EventCallbackFunctionWithArgNamedStringPtrType)(NamedStrings);
struct EventSubscription
{
public:
	EventSubscription(std::string eventName, EventCallbackFunctionPtrType callbackFunctionPtr);
public:
	std::string m_eventName;
	EventCallbackFunctionPtrType m_callBackFunctionPtr = nullptr;
};

struct EventSubscriptionWithArgNamedString
{
public:
	EventSubscriptionWithArgNamedString(std::string eventName, EventCallbackFunctionWithArgNamedStringPtrType callbackFunctionPtr);
public:
	std::string m_eventName;
	EventCallbackFunctionWithArgNamedStringPtrType m_callBackFunctionPtr = nullptr;
};

struct EventSubscriptionWithArg
{
public:
	EventSubscriptionWithArg(std::string eventName, EventCallbackFunctionWithArgPtrType callbackFunctionPtr);
public:
	std::string m_eventName;
	EventCallbackFunctionWithArgPtrType m_callBackFunctionPtr = nullptr;
};

struct EventSubscriptionDelegate
{
public:
	EventSubscriptionDelegate(std::string eventName, Delegate<NamedStrings> delegate);
public:
	std::string m_eventName;
	Delegate<NamedStrings> m_delegate;
};

class EventSystem
{
public:
	EventSystem();
	~EventSystem();
	void SubscribeToEvent(const std::string eventName, EventCallbackFunctionPtrType callbackFunctionPtr);
	void SubscribeToEvent(const std::string eventName, EventCallbackFunctionWithArgPtrType callbackFunctionPtr);
	void SubscribeToEvent(const std::string eventName, EventCallbackFunctionWithArgNamedStringPtrType callbackFunctionPtr);
	void SubscribeToEvent(const std::string eventName, Delegate<NamedStrings> callbackFunctionPtr);
	bool Fire(const std::string eventName);
	bool Fire(const std::string eventName, int param);
	bool Fire(const std::string eventName, NamedStrings params);
	void BanEvent(const std::string eventName);
	void AllowEvent(const std::string eventName);
	bool IsEventBanned(const std::string eventName) const;
	std::vector<std::string> GetEventList();
private:
	std::vector<EventSubscription* > m_eventSubscriptions;
	std::vector<EventSubscriptionWithArg* >m_eventSubscriptionsWithArgs;
	std::vector<EventSubscriptionWithArgNamedString* >m_eventSubscriptionsWithArgsNamedString;
	std::vector<EventSubscriptionDelegate* > m_eventSubscriptionsDelegate;
	std::vector<std::string> m_bannedEvents;
};