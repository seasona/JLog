#pragma once
#include <string>

#ifdef _MSC_VER
    #include <windows.h>
    #include <winsock.H>
#else
    #include <sys/types.h>
    #include <unistd.h>
#endif

/**
 * @brief 获取进程id和主机名称
 * 
 */
namespace ProcessInfo {

#ifdef _MSC_VER
    long pid();
#else
    pid_t pid();
#endif
std::string hostname();

}  // namespace ProcessInfo