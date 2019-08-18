#pragma once

#include <sys/types.h>
#include <string>
#include "noncopyable.h"

namespace JLog {

/**
 * @brief 最底层的文件类，封装了Log文件的打开、写入并在类析构的时候关闭
 * 文件，底层使用了标准IO，该append函数直接向文件写入
 *
 */
class AppendFile : public noncopyable {
public:
    explicit AppendFile(const std::string& filename);
    ~AppendFile();

    /**
     * @brief 刷新缓存区，将缓存区中的数据写入文件
     *
     */
    void flush();

    /**
     * @brief 往文件中写入日志文本
     *
     * @param logline 存入日志的文本
     * @param len 日志文本的大小
     */
    void append(const char* logline, const size_t len);

    /**
     * @brief 返回文件的大小和偏移量
     *
     * @return off_t 文件的大小和偏移量
     */
    off_t writtenBytes() const { return written_bytes_; }

private:
    size_t write(const char* logline, size_t len);

    FILE* fp_;
    off_t written_bytes_ = 0;
    char buffer_[64 * 1024];
};

}