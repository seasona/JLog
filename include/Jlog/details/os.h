/**
 * @file os.h
 * @author Ji Jie (396438446@qq.com)
 * @brief The different API in linux and windows
 * @version 0.1
 * @date 2019-09-27
 *
 * @copyright Copyright (c) 2019
 *
 */

#pragma once

#include <chrono>
#include <ctime>
#ifdef _WIN32
#include <io.h>
#include <process.h>
#include <windows.h>
#elif __linux__
#include <sys/types.h>
#include <syscall.h>
#include <unistd.h>
#endif

namespace Jlog {

namespace details {

namespace os {

// eol definition
#ifdef _WIN32
#define JLOG_EOL "\r\n"
#else
#define JLOG_EOL "\n"
#endif

/// usage like os::JLOG_EOL is wrong, because it will become os::"\n",
/// so it better not use macro in namespace
constexpr static const char* default_eol = JLOG_EOL;

// folder separator
#ifdef _WIN32
#define JLOG_FOLDER_SEP '\\'
#else
#define JLOG_FOLDER_SEP '/'
#endif

constexpr static const char default_folder_sep = JLOG_FOLDER_SEP;

/**
 * @brief Get the Process Id object
 * @return unsigned long Return current process id as unsigned long
 */
inline unsigned long getProcessId() {
#ifdef _WIN32
    return static_cast<unsigned long>(::GetCurrentProcessId());
#elif __linux__
    return static_cast<unsigned long>(::getpid());
#endif
}

/**
 * @brief It exists because the std::this_thread::get_id() is much slower
 * @return size_t Return current thread id as size_t
 */
inline size_t getThreadId() noexcept {
#ifdef _WIN32
    return static_cast<size_t>(::GetCurrentThreadId());
#elif __linux__
    return static_cast<size_t>(syscall(SYS_gettid));
#endif
}

/**
 * @brief The original localtime in C is much slower and non thread safe,
 * so use the os'function instead
 * @param t The current time, usually got by now()
 * @return std::tm The tm struct
 */
inline std::tm localTime(const time_t& t) {
#ifdef _WIN32
    std::tm p_time;
    localtime_s(&p_time, &t);
#elif __linux__
    std::tm p_time;
    localtime_r(&t, &p_time);
#endif
    return p_time;
}

/**
 * @brief get the time of now
 * @return std::chrono::system_clock::time_point the time of now
 */
inline std::chrono::system_clock::time_point now() noexcept {
    return std::chrono::system_clock::now();
}

}  // namespace os

}  // namespace details

}  // namespace Jlog