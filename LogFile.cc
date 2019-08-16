#include "LogFile.h"
#include <assert.h>
#include <stdio.h>
#include <time.h>
#include "FileUtil.h"
#include "ProcessInfo.h"

LogFile::LogFile(const std::string& basename, off_t roll_size,
                 int flush_interval, int check_every_N, bool thread_safe)
    : basename_(basename),
      roll_size_(roll_size),
      flush_interval_(flush_interval),
      check_every_N_(check_every_N),
      mutex_ptr_(thread_safe ? new std::mutex : NULL) {
    // 如果名称中含有'/'就报错
    assert(basename.find('/') == std::string::npos);
    rollFile();
}

// 滚动是无论当前写入量多大，超过了roll_size多少，先写入文件，
// 再开一个新文件，并没有对写入的数据进行分割
bool LogFile::rollFile() {
    time_t now = 0;
    std::string filename = getLogFileName(basename_, &now);
    time_t start = now / k_roll_per_seconds_ * k_roll_per_seconds_;

    // 时间偏差太小会导致写在同一个文件中
    if (now > last_roll_) {
        last_roll_ = now;
        last_flush_ = now;
        start_of_period_ = start;
        file_.reset(new AppendFile(filename));
        return true;
    }
    return false;
}

std::string LogFile::getLogFileName(const std::string& basename, time_t* now) {
    std::string filename;
    filename.reserve(basename.size() + 64);
    filename = basename;

    char timebuf[32];
    struct tm tm;
    *now = time(NULL);
    // 将当地时间转为tm结构体
    localtime_r(now, &tm);
    // 将tm转化为自定义格式的字符串
    strftime(timebuf, sizeof(timebuf), ".%Y:%m:%d-%H:%M:%S.", &tm);
    filename += timebuf;

    filename += ProcessInfo::hostname();

    char pidbuf[32];
    // 具有最大个数的安全的sprintf
    snprintf(pidbuf, sizeof(pidbuf), ".%d", ProcessInfo::pid());
    filename += pidbuf;

    filename += ".log";

    return filename;
}

void LogFile::append(const char* logline, int len) {
    if (mutex_ptr_) {
        std::lock_guard<std::mutex> lock(*mutex_ptr_);
        appendUnlocked(logline, len);
    } else {
        appendUnlocked(logline, len);
    }
}

void LogFile::flush() {
    if (mutex_ptr_) {
        std::lock_guard<std::mutex> lock(*mutex_ptr_);
        file_->flush();
    } else {
        file_->flush();
    }
}

void LogFile::appendUnlocked(const char* logline, int len) {
    file_->append(logline, len);

    // 写入数据数量大于roll_size
    if (file_->writtenBytes() > roll_size_) {
        rollFile();
    } else {
        ++count_;
        // 每写入N次检查一下是否需要刷新缓存区和更换日志文件
        if (count_ >= check_every_N_) {
            count_ = 0;
            time_t now = time(NULL);
            time_t this_period_ =
                now / k_roll_per_seconds_ * k_roll_per_seconds_;
            // 每过24小时更换一次日志文件
            if (this_period_ != start_of_period_) {
                rollFile();
            } else if (now - last_flush_ > flush_interval_) {
                last_flush_ = now;
                file_->flush();
            }
        }
    }
}