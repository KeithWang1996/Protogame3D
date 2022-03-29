#include "Engine/Core/Clock.hpp"
#include "Engine/Core/Time.hpp"
#include "Engine/Math/MathUtils.hpp"

static Clock gMasterClock(nullptr);

Clock::Clock()
	: Clock(GetMaster())
{}

Clock::Clock(Clock* parent)
{
	SetParent(parent);
}

Clock::~Clock()
{
	for (int childIndex = 0; childIndex < m_childern.size(); ++childIndex)
	{
		m_childern[childIndex]->SetParent(m_parent);
	}
	if (m_parent)
	{
		for (int PchildIndex = 0; PchildIndex < m_parent->m_childern.size(); ++PchildIndex)
		{
			if (m_parent->m_childern[PchildIndex] == this)
			{
				m_parent->m_childern.erase(m_parent->m_childern.begin() + PchildIndex);
				break;
			}
		}
	}
}

void Clock::Update(double deltaSeconds)
{
	if (IsPaused()) {
		deltaSeconds = 0.0;
		// DO NOT EARLY RETURN
	}
	else {
		deltaSeconds *= m_scale;
	}

	// update my clock concept of time
	m_deltaTime = (double)Clamp((float)deltaSeconds, (float)m_minFrameTime, (float)m_maxFrameTime);
	m_currentTime += deltaSeconds;

	for (int childIndex = 0; childIndex < m_childern.size(); ++childIndex)
	{
		m_childern[childIndex]->Update(deltaSeconds);
	}
}

void Clock::Reset()
{
	m_currentTime = 0.f;
}

void Clock::SetParent(Clock* clock)
{
	m_parent = clock;
	if (clock == nullptr)
	{
		return;
	}
	clock->m_childern.push_back(this);
}

void Clock::Pause()
{
	m_isPaused = true;
}

void Clock::Resume()
{
	m_isPaused = false;
}

void Clock::SwitchPause()
{
	m_isPaused = !m_isPaused;
}

void Clock::SetScale(double scale)
{
	m_scale = scale;
}

double Clock::GetTotalElapsedSeconds() const
{
	return m_currentTime;
}

double Clock::GetLastDeltaSeconds() const
{
	return m_deltaTime;
}

double Clock::GetScale() const
{
	return m_scale;
}

bool Clock::IsPaused() const
{
	return m_isPaused;
}

void Clock::SystemStartup()  // create/reset master clock
{
	gMasterClock.Reset();
}

void Clock::SystemShutdown()
{}

void Clock::BeginFrame()
{
	static double timePreviousFrame = GetCurrentTimeSeconds();
	double timeThisFrame = GetCurrentTimeSeconds();

	double dt = timeThisFrame - timePreviousFrame;
	timePreviousFrame = timeThisFrame;

	gMasterClock.Update(dt);
}

Clock* Clock::GetMaster()
{
	return &gMasterClock;
}