#pragma once

#ifdef _WIN32
#include <io.h>
#include <process.h>
#include <windows.h>
#elif __linux__
#include <syscall.h>
#include <unistd.h>
#endif

namespace OS {

/**
 * @brief Get the Process Id object
 *
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
 *
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
 * 
 * @param t The current time, usually got by now()
 * @return std::tm The tm struct 
 */
inline std::tm localTime(const time_t &t) {
#ifdef _WIN32
    std::tm p_time;
    localtime_s(&p_time, &t);
#elif __linux__
    std::tm p_time;
    localtime_r(&t, &p_time);
#endif
    return p_time;
}

}  // namespace OS
