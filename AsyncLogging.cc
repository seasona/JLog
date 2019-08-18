#include "AsyncLogging.h"
#include <assert.h>
#include <unistd.h>
#include <chrono>
#include <functional>
#include "LogFile.h"

namespace JLog {

AsyncLogging::AsyncLogging(const std::string& basename, off_t roll_size,
                           int flush_interval)
    : flush_interval_(flush_interval),
      roll_size_(roll_size),
      basename_(basename),
      current_buffer_(new Buffer),
      next_buffer_(new Buffer),
      running_(true),
      thread_(std::bind(&AsyncLogging::threadFunc, this)) {
    assert(basename_.size() > 1);
    current_buffer_->bzero();
    next_buffer_->bzero();
    buffers_.reserve(16);
}

AsyncLogging::~AsyncLogging() {
    running_ = false;
    thread_.join();
}

void AsyncLogging::append(const char* logline, int len) {
    std::lock_guard<std::mutex> lock(mutex_);
    if (current_buffer_->avail() > len) {
        current_buffer_->append(logline, len);
    } else {  //当前缓冲区已满
        buffers_.emplace_back(std::move(current_buffer_));
        if (next_buffer_) {
            // unique_ptr的赋值运算符只接受典型地由std::move生成的右值
            current_buffer_ = std::move(next_buffer_);
        } else {
            current_buffer_.reset(new Buffer);
        }
        current_buffer_->append(logline, len);
        cond_.notify_one();
    }
}

// 实际是用了4个缓冲区提高写入速度
void AsyncLogging::threadFunc() {
    LogFile output(basename_, roll_size_);
    BufferPtr new_buffer1(new Buffer);
    BufferPtr new_buffer2(new Buffer);
    new_buffer1->bzero();
    new_buffer2->bzero();
    BufferVector buffers_to_write;
    buffers_to_write.reserve(16);
    while (running_) {
        {
            std::unique_lock<std::mutex> lock(mutex_);

            // buffers不为空或等待时间超过3s即使当前缓冲区未满也写入
            cond_.wait_for(lock, std::chrono::seconds(flush_interval_),
                           [this] { return !buffers_.empty(); });

            // 一般是调用复制构造函数，但是unique_ptr的复制构造函数被删除了
            // 所以必须使用右值，调用移动构造函数
            buffers_.emplace_back(std::move(current_buffer_));

            // 因为new_buffer这类都是unique_ptr，所以当复制给另一个指针时，
            // 原有的指针被销毁
            current_buffer_ = std::move(new_buffer1);
            buffers_to_write.swap(buffers_);
            if (!next_buffer_) {
                next_buffer_ = std::move(new_buffer2);
            }
        }

        // 丢弃写入速度过快的垃圾日志
        if (buffers_to_write.size() > 25) {
            buffers_to_write.erase(buffers_to_write.begin() + 2,
                                   buffers_to_write.end());
        }

        // 正式在日志文件中写入
        for (const auto& buffer : buffers_to_write) {
            output.append(buffer->data(), buffer->length());
        }

        if (buffers_to_write.size() > 2) {
            buffers_to_write.resize(2);
        }

        // 两次emplace_back()保证了buffers_to_write中至少有两个Buffer
        if (!new_buffer1) {
            new_buffer1 = std::move(buffers_to_write.back());
            buffers_to_write.pop_back();
            new_buffer1->reset();
        }

        if (!new_buffer2) {
            new_buffer2 = std::move(buffers_to_write.back());
            buffers_to_write.pop_back();
            new_buffer2->reset();
        }

        buffers_to_write.clear();
        output.flush();
    }
    output.flush();
}

}