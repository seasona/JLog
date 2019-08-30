/**
 * @file blocking_queue.h
 * @author 季杰 (396438446@qq.com)
 * @brief 使用了boost库中的circular_buffer的堵塞队列
 * @version 0.1
 * @date 2019-08-28
 * 
 * @copyright Copyright (c) 2019
 * 
 */

#pragma once

#include <condition_variable>
#include <mutex>
#include <chrono>
#include "boost/circular_buffer.hpp"


/**
 * @brief 堵塞队列，也就是生产者消费者模型，使用了boost库中
 * 的circular_buffer
 * 
 * @tparam T 存储于circular_buffer中的类型
 */
template <typename T>
class BlockingQueue {
public:
    typedef T value_type;

    /**
     * @brief 构造函数
     * 
     * @param max_size circular_buffer的最大容量，超过后插入会将之前的数据删除
     */
    explicit BlockingQueue(size_t max_size) : que_(max_size) {}
    ~BlockingQueue() = default;

    void push(T &&item) {
        std::unique_lock<std::mutex> lock(mutex_);
        push_cv_.wait(lock, [this] { return !this->que_.full(); });
        que_.push_back(std::move(item));
        pop_cv_.notify_one();
    }

    /**
     * @brief 非等待的插入函数，如果队列已满就将circular_buffer中的旧数据直接删除
     * 
     * @param item 插入的数据
     */
    void pushNoWait(T &&item) {
        std::unique_lock<std::mutex> lock(mutex_);
        que_.push_back(std::move(item));
        pop_cv_.notify_one();
    }

    /**
     * @brief 移除并返回前端元素，如果一段时间内没等到也直接返回
     * 
     * @param wait_duration 等待的时间(ms)
     * @return T 返回前端元素
     */
    bool popFor(T &popped_item, std::chrono::milliseconds wait_duration) {
        std::unique_lock<std::mutex> lock(mutex_);
        if(!pop_cv_.wait_for(lock, wait_duration,
                         [this] { return !this->que_.empty(); })){
                             return false;
                         }
        popped_item= std::move(que_.front());
        que_.pop_front();
        push_cv_.notify_one();
        return true;
    }

    size_t size() const{
        std::lock_guard<std::mutex> lock(mutex_);
        return que_.size();
    }

private:
    mutable std::mutex mutex_;
    std::condition_variable push_cv_;
    std::condition_variable pop_cv_;

    boost::circular_buffer<T> que_;
};
