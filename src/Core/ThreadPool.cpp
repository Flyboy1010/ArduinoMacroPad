#include "Core/ThreadPool.h"

ThreadPool::ThreadPool(int threadsCount)
{
	m_workersCount = threadsCount;
	m_working = true;

	for (int i = 0; i < m_workersCount; i++)
		m_workers.push_back(std::thread(&ThreadPool::DoWork, this));
}

ThreadPool::~ThreadPool()
{
	m_working = false;

	// notify all threads to stop working

	m_condition.notify_all();

	// wait for all threads to finish

	for (auto& worker : m_workers)
		worker.join();
}

void ThreadPool::SubmitTask(const Task& task)
{
	// lock and push a task

	{
		std::scoped_lock lock(m_tasksMutex);
		m_tasks.push(task);
	}

	// notify that one task has been added

	m_condition.notify_one();
}

void ThreadPool::DoWork()
{
	while (m_working)
	{
		// wait until there are tasks to do or the threads are told to stop working
		// this is a non busy wait contrary to a while loop for example
		
		std::unique_lock lock(m_tasksMutex);
		m_condition.wait(lock, [&]()
			{
				return !m_tasks.empty() || !m_working;
			}); // after the wait we own the mutex

		// if there are any tasks

		if (!m_tasks.empty())
		{
			// get the task (as a copy)

			Task task = m_tasks.front();
			m_tasks.pop(); // remove the task from the queue

			// unlock the mutex

			lock.unlock();

			// do the task

			task();
		}
	}
}
