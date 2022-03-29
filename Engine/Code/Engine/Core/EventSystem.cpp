#include "Engine/Core/EventSystem.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Core/DevConsole.hpp"
#include "Engine/Core/NamedStrings.hpp"
#include "Engine/Core/NamedProperties.hpp"
#include <vector>

EventSubscription::EventSubscription(std::string eventName, EventCallbackFunctionPtrType callbackFunctionPtr)
	: m_eventName(eventName)
	, m_callBackFunctionPtr(callbackFunctionPtr)
{
}

EventSubscriptionWithArg::EventSubscriptionWithArg(std::string eventName, EventCallbackFunctionWithArgPtrType callbackFunctionPtr)
	: m_eventName(eventName)
	, m_callBackFunctionPtr(callbackFunctionPtr)
{
}

EventSubscriptionWithArgNamedString::EventSubscriptionWithArgNamedString(std::string eventName, EventCallbackFunctionWithArgNamedStringPtrType callbackFunctionPtr)
	: m_eventName(eventName)
	, m_callBackFunctionPtr(callbackFunctionPtr)
{}

EventSubscriptionDelegate::EventSubscriptionDelegate(std::string eventName, Delegate<NamedStrings> delegate)
	: m_eventName(eventName)
	, m_delegate(delegate)
{}

EventSystem::EventSystem()
{}

EventSystem::~EventSystem()
{
	for (int subscriptionIndex = 0; subscriptionIndex < m_eventSubscriptions.size(); ++subscriptionIndex)
	{
		delete m_eventSubscriptions[subscriptionIndex];
	}
	m_eventSubscriptions.clear();

	for (int subscriptionIndex = 0; subscriptionIndex < m_eventSubscriptionsDelegate.size(); ++subscriptionIndex)
	{
		delete m_eventSubscriptionsDelegate[subscriptionIndex];
	}
	m_eventSubscriptionsDelegate.clear();
}

void EventSystem::SubscribeToEvent(const std::string eventName, EventCallbackFunctionPtrType callbackFunctionPtr)
{
	EventSubscription* newSubscription = new EventSubscription(eventName, callbackFunctionPtr);
	m_eventSubscriptions.push_back(newSubscription);
}

void EventSystem::SubscribeToEvent(const std::string eventName, EventCallbackFunctionWithArgPtrType callbackFunctionPtr)
{
	EventSubscriptionWithArg* newSubscription = new EventSubscriptionWithArg(eventName, callbackFunctionPtr);
	m_eventSubscriptionsWithArgs.push_back(newSubscription);
}

void EventSystem::SubscribeToEvent(const std::string eventName, EventCallbackFunctionWithArgNamedStringPtrType callbackFunctionPtr)
{
	EventSubscriptionWithArgNamedString* newSubscription = new EventSubscriptionWithArgNamedString(eventName, callbackFunctionPtr);
	m_eventSubscriptionsWithArgsNamedString.push_back(newSubscription);
}

void EventSystem::SubscribeToEvent(const std::string eventName, Delegate<NamedStrings> callbackFunctionPtr)
{
	EventSubscriptionDelegate* newSubscription = new EventSubscriptionDelegate(eventName, callbackFunctionPtr);
	m_eventSubscriptionsDelegate.push_back(newSubscription);
}


bool EventSystem::Fire(const std::string eventName)
{
	if (IsEventBanned(eventName))
	{
// 		if (eventName == "quit")
// 		{
// 			g_theConsole->PrintString(Rgba8(255, 0, 0, 255), "DO NOT TRY TO ESCAPE.");
// 		}
		return false;
	}
	for (int subscriptionIndex = 0; subscriptionIndex < m_eventSubscriptions.size(); ++subscriptionIndex)
	{
		if (CompareTwoStrings(m_eventSubscriptions[subscriptionIndex]->m_eventName, eventName))
		{
			m_eventSubscriptions[subscriptionIndex]->m_callBackFunctionPtr();
			return true;
		}
	}
	return false;
}

bool EventSystem::Fire(const std::string eventName, int param)
{
	if (IsEventBanned(eventName))
	{
		return false;
	}
	for (int subscriptionIndex = 0; subscriptionIndex < m_eventSubscriptionsWithArgs.size(); ++subscriptionIndex)
	{
		if (CompareTwoStrings(m_eventSubscriptionsWithArgs[subscriptionIndex]->m_eventName, eventName))
		{
			m_eventSubscriptionsWithArgs[subscriptionIndex]->m_callBackFunctionPtr(param);
			return true;
		}
	}
	return false;
}

bool EventSystem::Fire(const std::string eventName, NamedStrings params)
{
	if (IsEventBanned(eventName))
	{
		return false;
	}
	for (int subscriptionIndex = 0; subscriptionIndex < m_eventSubscriptionsWithArgsNamedString.size(); ++subscriptionIndex)
	{
		if (CompareTwoStrings(m_eventSubscriptionsWithArgsNamedString[subscriptionIndex]->m_eventName, eventName))
		{
			m_eventSubscriptionsWithArgsNamedString[subscriptionIndex]->m_callBackFunctionPtr(params);
			return true;
		}
	}
	return false;
}

void EventSystem::BanEvent(const std::string eventName)
{
	m_bannedEvents.push_back(eventName);
}
void EventSystem::AllowEvent(const std::string eventName)
{
	for (int i = 0; i < m_bannedEvents.size(); ++i)
	{
		if (m_bannedEvents[i] == eventName)
		{
			m_bannedEvents.erase(m_bannedEvents.begin() + i);
		}
	}
}
bool EventSystem::IsEventBanned(const std::string eventName) const
{
	for (int i = 0; i < m_bannedEvents.size(); ++i)
	{
		if (m_bannedEvents[i] == eventName)
		{
			return true;
		}
	}
	return false;
}

std::vector<std::string> EventSystem::GetEventList()
{
	std::vector<std::string> eventList;
	for (int eventIndex = 0; eventIndex < m_eventSubscriptions.size(); ++eventIndex)
	{
		eventList.push_back(m_eventSubscriptions[eventIndex]->m_eventName);
	}
	for (int eventIndex = 0; eventIndex < m_eventSubscriptionsWithArgs.size(); ++eventIndex)
	{
		eventList.push_back(m_eventSubscriptionsWithArgs[eventIndex]->m_eventName);
	}
	for (int eventIndex = 0; eventIndex < m_eventSubscriptionsWithArgsNamedString.size(); ++eventIndex)
	{
		eventList.push_back(m_eventSubscriptionsWithArgsNamedString[eventIndex]->m_eventName);
	}
	for (int eventIndex = 0; eventIndex < m_eventSubscriptionsDelegate.size(); ++eventIndex)
	{
		eventList.push_back(m_eventSubscriptionsDelegate[eventIndex]->m_eventName);
	}
	return eventList;
}