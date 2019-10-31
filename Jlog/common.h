#pragma once

#include <atomic>
#include <chrono>
#include <functional>
#include <memory>
#include <string_view>
#include "fmt/fmt.h"

#define JLOG_HEADER_ONLY

namespace Jlog {

namespace sinks {
class Sink;
}  // namespace sinks

namespace level {
// Log level enum
enum level_enum { k_trace, k_debug, k_info, k_warn, k_err, k_critical, k_off };

}  // namespace level

struct SourceLoc {
    constexpr SourceLoc() = default;
    constexpr SourceLoc(const char *filename_in, int line_in,
                        const char *funcname_in)
        : filename(filename_in), line(line_in), funcname(funcname_in) {}
    constexpr bool empty() const noexcept { return line == 0; }

    const char *filename = nullptr;
    int line = 0;
    const char *funcname = nullptr;
};

using level_t = std::atomic<int>;
using string_view_t = std::basic_string_view<char>;
using wstring_view_t = std::basic_string_view<wchar_t>;
using log_clock = std::chrono::system_clock;
using sink_ptr = std::shared_ptr<sinks::Sink>;
// TODO change std::string to std::string_view
using err_handler = std::function<void(const std::string &err_msg)>;
// the basic_memory_buffer in fmt is just like the std::vector<char>
using memory_buf_t = fmt::basic_memory_buffer<char, 250>;

}  // namespace Jlog