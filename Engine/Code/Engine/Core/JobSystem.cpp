#include "Engine/Core/JobSystem.hpp"
#include "Engine/Core/DevConsole.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Core/Rgba8.hpp"
#include <string>
#include <iostream>
#include <Windows.h>
Job::Job()
{
	static int s_nextJobID = 1;
	m_jobID = s_nextJobID++;
}


void ExampleJob::Execute()
{
	g_theConsole->PrintString(Rgba8::WHITE, "Job " + std::to_string(m_jobID) + " start execution.");
	Sleep(5000);
}

void ExampleJob::OnCompleteCallBack()
{
	g_theConsole->PrintString(Rgba8::WHITE, "Job " + std::to_string(m_jobID) + " completed.");
}

void JobSystemWorkerThread::WorkerThreadMain()
{
	g_theConsole->PrintString(Rgba8::WHITE, "Thread " + std::to_string(m_threadID) + " start.");
	while (!m_system->m_isQuitting)
	{
		m_system->m_jobsQueuedMutex.lock();
		if (!m_system->m_jobsQueued.empty())
		{
			Job* jobAtFrontOfQueue = m_system->m_jobsQueued.front();
			m_system->m_jobsQueued.pop_front();
			m_system->m_jobsQueuedMutex.unlock();
			StartJob(jobAtFrontOfQueue);
			jobAtFrontOfQueue->Execute(); // might take a while

			Job* jobCompleted = m_system->PopJobFromRunningByIndex(jobAtFrontOfQueue->m_jobID);

			m_system->m_jobsCompleteMutex.lock();//job completed
			m_system->m_jobsCompleted.push_back(jobCompleted);
			m_system->m_jobsCompleteMutex.unlock();
		}
		else
		{
			m_system->m_jobsQueuedMutex.unlock();
			std::this_thread::sleep_for(std::chrono::microseconds(50));
		}
	}
	g_theConsole->PrintString(Rgba8::WHITE, "Thread " + std::to_string(m_threadID) + " exiting.");
}

void JobSystemWorkerThread::StartJob(Job* job)
{
	m_system->m_jobsRunningMutex.lock();
	m_system->m_jobsRunning.push_back(job);
	m_system->m_jobsRunningMutex.unlock();
}

void JobSystemWorkerThread::FinishJob(Job* job)
{
	m_system->m_jobsCompleteMutex.lock();
	m_system->m_jobsCompleted.push_back(job);
	m_system->m_jobsCompleteMutex.unlock();
}

JobSystem::~JobSystem()
{
	ShutDown();
}

void JobSystem::CreateWorkerThread(int threadID)
{
	m_workerThreads.push_back(new JobSystemWorkerThread(threadID, this));
}

void JobSystem::CreateWorkerThreads(int numThread)
{
	for(int i = 0; i < numThread; ++i)
	{
		CreateWorkerThread(i);
	}
}

void JobSystem::StopWorkerThreads()
{
	for (int i = 0; i < m_workerThreads.size(); ++i)
	{
		m_workerThreads[i]->m_threadObject->join();
	}

	for (int i = 0; i < m_workerThreads.size(); ++i)
	{
		delete m_workerThreads[i];
	}
}

void JobSystem::PostJob(Job* job)
{
	if (m_isQuitting)
	{
		return;
	}
	m_jobsQueuedMutex.lock();
	m_jobsQueued.push_back(job);
	m_jobsQueuedMutex.unlock();
}

void JobSystem::ClaimAndDeleteAllCompletedJobs()
{
	std::deque<Job*> claimedJobs;

	m_jobsCompleteMutex.lock();
	m_jobsCompleted.swap(claimedJobs);
	m_jobsCompleteMutex.unlock();

	for (auto iter = claimedJobs.begin(); iter != claimedJobs.end(); ++iter)
	{
		Job* job = *iter;
		job->OnCompleteCallBack();
		delete job;
	}
}

Job* JobSystem::PopJobFromRunningByIndex(int id)
{
	m_jobsRunningMutex.lock();
	for (auto iter = m_jobsRunning.begin(); iter != m_jobsRunning.end(); ++iter)
	{
		Job* job = *iter;
		if (job->m_jobID == id)
		{
			m_jobsRunning.erase(iter);
			m_jobsRunningMutex.unlock();
			return job;
		}
	}
	m_jobsRunningMutex.unlock();
	return nullptr;
}

void JobSystem::ShutDown()
{
	if (m_isQuitting)
	{
		return;
	}
	m_isQuitting = true;
	StopWorkerThreads();
}