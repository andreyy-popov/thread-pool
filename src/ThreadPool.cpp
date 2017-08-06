//============================================================================
// Name        : ThreadPool.cpp
// Date        : 06.08.2017
// Author      : Andrey Popov
// Version     :
// Copyright   : All rights reserved
// Description :
//============================================================================

#include "ThreadPool.h"
#include <algorithm>

// Конструктор.
ThreadPool::ThreadPool(unsigned int threadCount, unsigned int maxCallQueueSize) : exitFlag(false), maxCallQueueSize(maxCallQueueSize) {
  while (threadCount--) {
    threads.emplace_back(
      [&] {
        std::unique_lock<std::mutex> autoLock(lock);
        while (!exitFlag) {
          if (calls.empty()) {
            event.wait(autoLock);
          } else {
            std::function<void()> call = calls.front();
            calls.pop();
            autoLock.unlock();
            call();
            autoLock.lock();
          }
        }
      }
    );
  }
}

// Деструктор.
ThreadPool::~ThreadPool() {
  {
    std::lock_guard<std::mutex> autoLock(lock);
    exitFlag = true;
  }
  event.notify_all();
  std::for_each(threads.begin(), threads.end(), std::bind(&std::thread::join, std::placeholders::_1));
}
