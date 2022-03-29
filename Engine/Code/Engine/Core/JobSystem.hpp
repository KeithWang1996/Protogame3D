#pragma once
#include <mutex>
#include <atomic>
#include <vector>
#include <deque>
class JobSystem;
class Job
{
public:
	Job();
	virtual ~Job() {}
	virtual void Execute() = 0;
	virtual void OnCompleteCallBack() {};

	int m_jobID = 0;
};

class ExampleJob : public Job
{
	virtual void Execute() override;
	virtual void OnCompleteCallBack() override;
};

class JobSystemWorkerThread
{
public:
	void WorkerThreadMain();
	void StartJob(Job* job);
	void FinishJob(Job* job);
	JobSystemWorkerThread(int threadID, JobSystem* system)
		: m_threadID(threadID)
		, m_system(system)
	{
		m_threadObject = new std::thread(&JobSystemWorkerThread::WorkerThreadMain, this);
	}

	~JobSystemWorkerThread()
	{
		delete m_threadObject;
	}
public:
	std::thread* m_threadObject = nullptr;
	int m_threadID = 0;
	JobSystem* m_system = nullptr;
};

class JobSystem
{
public:
	JobSystem() = default;
	~JobSystem();//handle shut down case
	void CreateWorkerThread(int threadID);
	void CreateWorkerThreads(int numThread);
	void StopWorkerThreads();
	void PostJob(Job* job);
	void ClaimAndDeleteAllCompletedJobs();//
	Job* PopJobFromRunningByIndex(int id);
	void ShutDown();

private:
	std::deque< Job* >	m_jobsQueued;
	std::deque< Job* >  m_jobsRunning;
	std::deque< Job* >	m_jobsCompleted;
	std::mutex			m_jobsQueuedMutex;
	std::mutex			m_jobsRunningMutex;
	std::mutex			m_jobsCompleteMutex;
	std::atomic<bool>	m_isQuitting = false;
	std::vector< JobSystemWorkerThread* >		m_workerThreads;

	friend class JobSystemWorkerThread;
};