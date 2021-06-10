// Use of this source code is governed by a BSD-style license
// that can be found in the License file.
//
// Author: Shuo Chen (chenshuo at chenshuo dot com)

#ifndef MUDUO_BASE_CONDITION_H
#define MUDUO_BASE_CONDITION_H

#include "muduo/base/Mutex.h"

#include <pthread.h>

namespace muduo
{
  // 条件变量
  class Condition : noncopyable
  {
  public:
    // 初始化一个条件变量
    explicit Condition(MutexLock &mutex)
        : mutex_(mutex)
    {
      MCHECK(pthread_cond_init(&pcond_, NULL));
    }

    ~Condition()
    {
      MCHECK(pthread_cond_destroy(&pcond_));
    }

    void wait()
    {
      MutexLock::UnassignGuard ug(mutex_);
      MCHECK(pthread_cond_wait(&pcond_, mutex_.getPthreadMutex()));
    }

    //
    bool waitForSeconds(double seconds);

    void notify()
    {
      MCHECK(pthread_cond_signal(&pcond_));
    }

    void notifyAll()
    {
      MCHECK(pthread_cond_broadcast(&pcond_));
    }

  private:
    MutexLock &mutex_;
    pthread_cond_t pcond_;
  };

} // namespace muduo

#endif // MUDUO_BASE_CONDITION_H
