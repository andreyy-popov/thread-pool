//============================================================================
// Name        : ThreadPool.h
// Date        : 06.08.2017
// Author      : Andrey Popov
// Version     :
// Copyright   : All rights reserved
// Description :
//============================================================================

#ifndef _THREAD_POOL_H_
#define _THREAD_POOL_H_

#include <thread>
#include <mutex>
#include <condition_variable>
#include <queue>
#include <list>

class ThreadPool {
  bool exitFlag;
  unsigned int maxCallQueueSize;
  std::mutex lock;
  std::condition_variable event;
  std::queue<std::function<void()>> calls;
  std::list<std::thread> threads;
public:
  // Конструктор.
  ThreadPool(unsigned int threadCount = std::thread::hardware_concurrency(), unsigned int maxCallQueueSize = 1024);
  // Деструктор.
  ~ThreadPool();
  // Асинхронный вызов.
  template<class Function, class... Args>
  bool AsyncCall(Function&& fn, Args&&... args);
};

// Асинхронный вызов.
template<class Function, class... Args>
bool ThreadPool::AsyncCall(Function&& fn, Args&&... args) {
  {
    std::lock_guard<std::mutex> autoLock(lock);
    if (calls.size() >= maxCallQueueSize) {
      return false;
    }
    calls.emplace([fn, args...] { fn(args...); });
  }
  return (event.notify_one(), true);
}

#endif // _THREAD_POOL_H_
