#pragma once

#include <atomic>
#include <condition_variable>
#include <memory>
#include <mutex>
#include <thread>
#include <vector>
#include "LogStream.h"

namespace JLog {

/**
 * @brief 异步日志类，分为前后端，前端append()写入缓冲区，缓冲区满了之后
 * 写入后端，后端是一个线程，将缓冲区写入文件。使用双缓冲区提高写入速度，作为
 * 单例类使用
 *
 */
class AsyncLogging : public noncopyable {
public:
    /**
     * @brief 获取单例AsyncLogging，使用了原子栅栏确保正确的双重检查锁模式
     * 
     * @param basename 日志文件基础名
     * @param roll_size 滚动大小
     * @param flush_interval 刷新时间
     * @return AsyncLogging* 返回单例
     */
    static AsyncLogging* getInstance(const std::string& basename, off_t roll_size,
                              int flush_interval = 3);

    /**
     * @brief 线程安全的前端写入函数
     *
     * @param logline 写入数据
     * @param len 写入数据大小
     */
    void append(const char* logline, int len);

private:
    /**
     * @brief 构造函数，注意Asynclogging一般只构造一个，作为单例使用
     *
     * @param basename 日志文件基础名
     * @param roll_size 日志文件滚动大小
     * @param flush_interval 缓冲区最迟写入时间
     */
    AsyncLogging(const std::string& basename, off_t roll_size,
                 int flush_interval = 3);
    ~AsyncLogging();
    static std::atomic<AsyncLogging*> instance_;

    void threadFunc();
    typedef FixedBuffer<k_large_buffer> Buffer;
    typedef std::vector<std::unique_ptr<Buffer>> BufferVector;
    typedef std::unique_ptr<Buffer> BufferPtr;

    const int flush_interval_;
    const off_t roll_size_;
    const std::string basename_;

    std::atomic<bool> running_;  ///< 原子操作，用于销毁线程
    std::thread thread_;
    static std::mutex mutex_;
    std::condition_variable cond_;
    BufferPtr current_buffer_;
    BufferPtr next_buffer_;
    BufferVector buffers_;
};

}  // namespace JLog