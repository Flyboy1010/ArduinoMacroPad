#pragma once

#include <thread>
#include <condition_variable>
#include <functional>
#include <queue>
#include <mutex>
#include <atomic>

using Task = std::function<void()>;

class ThreadPool
{
public:
	ThreadPool(int threadsCount = std::thread::hardware_concurrency());
	~ThreadPool();

	int GetWorkersCount() const { return m_workersCount; }

	void SubmitTask(const Task& task);

private:
	void DoWork();

private:
	int m_workersCount;
	std::atomic<bool> m_working;
	std::vector<std::thread> m_workers;
	std::queue<Task> m_tasks;
	std::mutex m_tasksMutex;
	std::condition_variable m_condition;
};