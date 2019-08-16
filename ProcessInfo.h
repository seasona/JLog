#pragma once

#include <sys/types.h>
#include <string>

namespace ProcessInfo {

pid_t pid();
std::string hostname();

}  // namespace ProcessInfo