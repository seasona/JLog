#pragma once
#include <condition_variable>
#include <mutex>
#include "noncopyable.h"

/**
 * @brief 倒计时器类
 * 
 */
class CountDownLatch : public noncopyable {
public:
    CountDownLatch(int count);
    ~CountDownLatch() = default;

    void countWait();
    void countDown();
    int getCount() const;

private:
    mutable std::mutex mutex_;
    std::condition_variable cv_;
    int count_;
};