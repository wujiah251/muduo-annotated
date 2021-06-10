// Use of this source code is governed by a BSD-style license
// that can be found in the License file.
//
// Author: Shuo Chen (chenshuo at chenshuo dot com)

#ifndef MUDUO_BASE_COUNTDOWNLATCH_H
#define MUDUO_BASE_COUNTDOWNLATCH_H

#include "muduo/base/Condition.h"
#include "muduo/base/Mutex.h"

namespace muduo
{
  // 一个同步辅助类，在完成一组正在其他线程中执行的操作之前
  // 它允许一个或多个线程一直等待。
  class CountDownLatch : noncopyable
  {
  public:
    explicit CountDownLatch(int count);

    void wait();

    void countDown();

    int getCount() const;

  private:
    mutable MutexLock mutex_;
    Condition condition_ GUARDED_BY(mutex_);
    int count_ GUARDED_BY(mutex_);
  };

} // namespace muduo
#endif // MUDUO_BASE_COUNTDOWNLATCH_H
