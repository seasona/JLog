#pragma once

#include <mutex>
#include <Jlog/details/null_mutex.h>

namespace Jlog {

namespace details{

/**
 * @brief 终端的锁，用于控制多线程sink互斥
 */
struct ConsoleMutex{
    using mutex_t = std::mutex;
    static mutex_t& mutex(){
        static mutex_t s_mutex;
        return s_mutex;
    }
};

struct ConsoleNullMutex{
    using mutex_t = NullMutex;
    static mutex_t& mutex(){
        static mutex_t s_mutex;
        return s_mutex;
    }
};

} // namespace details

} // namespace