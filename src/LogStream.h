#pragma once

#include <string.h>
#include <string>
#include "noncopyable.h"

namespace JLog {

const int k_small_buffer = 4000;        //4KB
const int k_large_buffer = 4000 * 1000; //4MB

/**
 * @brief 缓冲区
 *
 * @tparam SIZE 根据模板设置缓冲区大小
 */
template <int SIZE>
class FixedBuffer : public noncopyable {
public:
    FixedBuffer() : cur_(data_) {}
    ~FixedBuffer() = default;

    const char* data() const { return data_; }
    int length() const { return static_cast<int>(cur_ - data_); }

    char* current() { return cur_; }
    int avail() const { return static_cast<int>(end() - cur_); }
    void add(size_t len) { cur_ += len; }

    void reset() { cur_ = data_; }
    void bzero() { memset(data_, 0, sizeof(data_)); }

    /**
     * @brief 往缓存区中复制数据
     *
     * @param buf 待复制的数据区
     * @param len 待复制的数据大小
     */
    void append(const char* buf, size_t len) {
        if (avail() > static_cast<int>(len)) {
            memcpy(cur_, buf, len);
            cur_ += len;
        }
    }

private:
    const char* end() const { return data_ + sizeof(data_); }
    char data_[SIZE];  ///< 缓存区
    char* cur_;        ///< 指向空闲部分的指针
};

/**
 * @brief 封装了各种类型的重载的<<来进行输出，自带一个缓存区
 *
 */
class LogStream : public noncopyable {
public:
    typedef FixedBuffer<k_small_buffer> Buffer;

    LogStream() = default;
    ~LogStream() = default;

    void append(const char* data, int len) { buffer_.append(data, len); }
    const Buffer& buffer() const { return buffer_; }
    void resetBuffer() { buffer_.reset(); }

    LogStream& operator<<(bool v);
    LogStream& operator<<(short);
    LogStream& operator<<(unsigned short);
    LogStream& operator<<(int);
    LogStream& operator<<(unsigned int);
    LogStream& operator<<(long);
    LogStream& operator<<(unsigned long);
    LogStream& operator<<(long long);
    LogStream& operator<<(unsigned long long);
    // TODO 使用浮点数高效无错误转为字符串的grisu3算法
    LogStream& operator<<(float v);
    LogStream& operator<<(double);
    LogStream& operator<<(long double);
    LogStream& operator<<(char v);
    LogStream& operator<<(const char* str);
    LogStream& operator<<(const unsigned char* str);
    LogStream& operator<<(const std::string& v);
    LogStream& operator<<(const void*);

private:
    void staticCheck();

    /**
     * @brief 将数值类型转为字符串并进行复制
     *
     * @tparam T 十进制数值类型
     */
    template <typename T>
    void formatInteger(T);

    Buffer buffer_;
    static const int k_max_size = 32;  // 32位代表double等的有效位数+12位小数位
};

}