#include "FileUtil.h"
#include <assert.h>
#include <stdio.h>

namespace JLog {

AppendFile::AppendFile(const std::string& filename)
    : fp_(fopen(filename.c_str(), "a")) {
    //// setbuffer的作用就是对读写文件流设立一个缓存区(linux)
    //// setbuffer(fp_, buffer_, sizeof(buffer_));
    // 采用c内置函数setvbuf
    setvbuf(fp_, buffer_, _IOFBF, sizeof(buffer_));
}

AppendFile::~AppendFile() { fclose(fp_); }

void AppendFile::flush() {
    // 刷新缓存区，将其中的数据写入文件流
    fflush(fp_);
}

size_t AppendFile::write(const char* logline, size_t len) {
    //// fwrite的线程不安全形式，比fwrite更快
    //// return fwrite_unlocked(logline, 1, len, fp_);
    // win下没有fwrite_unlocked，为了跨平台采用fwrite
    return fwrite(logline, 1, len, fp_);
}

void AppendFile::append(const char* logline, const size_t len) {
    size_t n = this->write(logline, len);
    size_t remain = len - n;  // fwrite后剩下没写入的字符数
    while (remain > 0) {
        size_t x = this->write(logline + n, remain);
        if (x == 0) {
            // ferror检查给定流的错误，若文件流已出现错误为非零值，否则为0
            int err = ferror(fp_);
            if (err) {
                fprintf(stderr, "AppendFile::append() failed!\n");
            } else {
                fprintf(stderr, "File stream failed!\n");
            }
            break;
        }
        n += x;
        remain = len - n;
    }
    written_bytes_ += len;
}

}  // namespace JLog
