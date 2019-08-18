#pragma once

#include <memory>
#include <mutex>
#include <string>
#include "FileUtil.h"
#include "noncopyable.h"


namespace JLog {

/**
 * @brief LogFile类对FileUtil类进行了封装，可以完成对日志文件名的
 * 自动记录，以及对于日志写入数据大小以及每24小时自动更换日志文件
 *
 */
class LogFile : public noncopyable {
public:
    /**
     * @brief 构造函数
     *
     * @param basename 自定义的日志文件名
     * @param roll_size 日志文件中已经写入的数据大小
     * @param flush_interval 刷新缓存区的时间间隔
     * @param check_ever_N 每写入N次检查一下
     * @param thread_safe 是否线程安全
     */
    LogFile(const std::string& basename, off_t roll_size,
            int flush_interval = 3, int check_ever_N = 1024,
            bool thread_safe = false);
    ~LogFile() = default;

    void append(const char* logline, int len);
    void flush();

    /**
     * @brief 更换新的日志文件
     *
     * @return true 更换成功
     * @return false 更换失败，哪里出现问题需要处理
     */
    bool rollFile();

private:
    void appendUnlocked(const char* logline, int len);
    /**
     * @brief 获取日志文件的名称，格式为basename.localtime.hostname.pid.log
     *
     * @param basename 自定义的文件名称
     * @param now 当前时间
     * @return std::string 日志文件名称
     */
    static std::string getLogFileName(const std::string& basename, time_t* now);

    const std::string basename_;
    const off_t roll_size_;     ///< 日志文件的最大写入量
    const int flush_interval_;  ///< 刷新缓存区的最大时间间隔
    const int check_every_N_;   ///< 每写入N次检查一下

    int count_ = 0;

    std::unique_ptr<std::mutex> mutex_ptr_;
    time_t start_of_period_ = 0;  ///< 每24小时的记录点
    time_t last_roll_ = 0;
    time_t last_flush_ = 0;
    std::unique_ptr<AppendFile> file_;

    const static int k_roll_per_seconds_ = 60 * 60 * 24;
};

}