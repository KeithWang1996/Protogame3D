#include "Engine/Core/Timer.hpp"
#include "Engine/Core/Clock.hpp"

void Timer::SetSeconds(Clock* clock, double timeToWait)
{
	m_clock = clock;
	SetSeconds(timeToWait);
}

void Timer::SetSeconds(double timeToWait)
{
	m_startSeconds = m_clock->GetTotalElapsedSeconds();
	m_durationSeconds = timeToWait;
}

void Timer::Reset()
{
	m_durationSeconds = m_clock->GetTotalElapsedSeconds();
}

void Timer::Stop()
{
	m_durationSeconds = -1.0;
}

double Timer::GetElapsedSeconds() const
{
	return m_clock->GetTotalElapsedSeconds() - m_startSeconds;
}

double Timer::GetSecondsRemaining() const
{
	return m_durationSeconds - GetElapsedSeconds();
}

bool Timer::HasElapsed() const
{
	double currentTime = m_clock->GetTotalElapsedSeconds();
	return (currentTime > (m_startSeconds + m_durationSeconds));
}

bool Timer::CheckAndDecrement()
{
	if (HasElapsed())
	{
		m_startSeconds += m_durationSeconds;
		return true;
	}
	else
	{
		return false;
	}
}

int Timer::CheckAndDecrementAll()
{
	int numInterval = 0;
	while (CheckAndDecrement())
	{
		numInterval++;
	}
	return numInterval;
}

bool Timer::CheckAndReset()
{
	if (HasElapsed())
	{
		Reset();
		return true;
	}
	else
	{
		return false;
	}
}

bool Timer::IsRunning() const
{
	return m_durationSeconds > 0.f;
}
