//============================================================================
// Name        : main.cpp
// Date        : 06.08.2017
// Author      : Andrey Popov
// Version     :
// Copyright   : All rights reserved
// Description :
//============================================================================

#include "ThreadPool.h"
#include <functional>
#include <iostream>
#include <mutex>

int main() {
  std::mutex lock;
  auto f1 = [&lock] (int i) {
    std::this_thread::sleep_for(std::chrono::seconds(1));
    std::lock_guard<std::mutex> guard(lock);
    std::cout << std::this_thread::get_id() << " f(" << i << ")" << std::endl;
  };
  auto f2 = [&lock] (int i, int j) {
    std::this_thread::sleep_for(std::chrono::seconds(2));
    std::lock_guard<std::mutex> guard(lock);
    std::cout << std::this_thread::get_id() << " f(" << i << ", " << j << ")" << std::endl;
  };
  ThreadPool tp(10);
  for (int i = 0; i < 50; ++i) {
    tp.AsyncCall(f1, i);
    tp.AsyncCall(f2, i, i + 1);
  }
  std::this_thread::sleep_for(std::chrono::seconds(20));
  return 0;
}
