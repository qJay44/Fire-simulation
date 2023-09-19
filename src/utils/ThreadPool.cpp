#include "ThreadPool.h"

void ThreadPool::wait() {
  std::this_thread::yield();
}

void ThreadPool::threadLoop() {
  while (true) {
    if (remainingTasks > 0) wait();

    std::function<void()> job;
    {
      std::unique_lock<std::mutex> lock(queueMutex);
      mutexCondition.wait(lock, [this] {
        return !jobs.empty() || shouldTerminate;
      });
      if (shouldTerminate)
        return;
      job = jobs.front();
      jobs.pop();
    }
    job();
    remainingTasks--;
  }
}

void ThreadPool::start() {
  const uint32_t numThreads = std::thread::hardware_concurrency();
  for (uint32_t ii = 0; ii < numThreads; ++ii)
    threads.emplace_back(std::thread(&ThreadPool::threadLoop, this));
}

void ThreadPool::start(uint32_t restriction = 1) {
  const uint32_t numThreads = std::min(restriction, std::thread::hardware_concurrency());
  for (uint32_t ii = 0; ii < numThreads; ++ii)
    threads.emplace_back(std::thread(&ThreadPool::threadLoop, this));
}

void ThreadPool::queueJob(const std::function<void()>& job) {
  {
    std::unique_lock<std::mutex> lock(queueMutex);
    jobs.push(job);
    remainingTasks++;
  }
  mutexCondition.notify_one();
}

void ThreadPool::waitForCompletion() const {
  while (remainingTasks > 0)
    wait();
}

void ThreadPool::stop() {
  {
    std::unique_lock<std::mutex> lock(queueMutex);
    shouldTerminate = true;
  }
  mutexCondition.notify_all();

  for (std::thread& activeThread : threads)
    activeThread.join();

  threads.clear();
}

// Threads amount
const int ThreadPool::size() const {
  return threads.size();
}

