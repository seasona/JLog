#pragma once
#include <mutex>
#include "Jlog/details/console_mutex.h"
#include "Jlog/details/os.h"
#include "Jlog/details/pattern_formatter.h"
#include "Jlog/sinks/sink.h"

namespace Jlog {

namespace sinks {

template <typename Mutex>
class AnsicolorSink : public Sink {
public:
    using mutex_t = typename Mutex::mutex_t;
    AnsicolorSink(FILE *target_file);
    ~AnsicolorSink() override = default;

    AnsicolorSink(const AnsicolorSink &other) = delete;
    AnsicolorSink &operator=(const AnsicolorSink &other) = delete;

    void setColor(level::level_enum color_level, string_view_t color);
    void setShouldColor(bool should_color);
    bool shouldColor();

    void log(const details::LogMessage &msg) override;
    void flush() override;
    // void setFormatter(std::unique_ptr<Jlog::Formatter> sink_formatter)
    // override;

    // Formatting codes
    const string_view_t reset = "\033[m";
    const string_view_t bold = "\033[1m";
    const string_view_t dark = "\033[2m";
    const string_view_t underline = "\033[4m";
    const string_view_t blink = "\033[5m";
    const string_view_t reverse = "\033[7m";
    const string_view_t concealed = "\033[8m";
    const string_view_t clear_line = "\033[K";

    // Foreground colors
    const string_view_t black = "\033[30m";
    const string_view_t red = "\033[31m";
    const string_view_t green = "\033[32m";
    const string_view_t yellow = "\033[33m";
    const string_view_t blue = "\033[34m";
    const string_view_t magenta = "\033[35m";
    const string_view_t cyan = "\033[36m";
    const string_view_t white = "\033[37m";

    // Background colors
    const string_view_t on_black = "\033[40m";
    const string_view_t on_red = "\033[41m";
    const string_view_t on_green = "\033[42m";
    const string_view_t on_yellow = "\033[43m";
    const string_view_t on_blue = "\033[44m";
    const string_view_t on_magenta = "\033[45m";
    const string_view_t on_cyan = "\033[46m";
    const string_view_t on_white = "\033[47m";

    // Bold colors
    const string_view_t yellow_bold = "\033[33m\033[1m";
    const string_view_t red_bold = "\033[31m\033[1m";
    const string_view_t bold_on_red = "\033[1m\033[41m";

private:
    FILE *target_file_;
    mutex_t &mutex_;
    bool should_do_colors_;
    std::unique_ptr<Formatter> formatter_;
    std::unordered_map<level::level_enum, string_view_t, level::level_hasher>
        colors_;
    void printColorCode(const string_view_t &color_code);
    void printRange(const memory_buf_t &formatted, size_t start, size_t end);
};

template <typename Mutex>
class AnsicolorStdoutSink : public AnsicolorSink<Mutex> {
public:
    explicit AnsicolorStdoutSink() : AnsicolorSink<Mutex>(stdout) {}
};

using ansicolor_stdout_sink_mt = AnsicolorStdoutSink<details::ConsoleMutex>;
using ansicolor_stdout_sink_st = AnsicolorStdoutSink<details::ConsoleNullMutex>;

/// at first I put include path in namespace, but it can't find the path
/// location so the include path can't put in namespace
// #include "Jlog/details/pattern_formatter.h"
// #include "Jlog/sinks/sink.h"

template <typename Mutex>
inline AnsicolorSink<Mutex>::AnsicolorSink(FILE *target_file)
    : target_file_(target_file),
      mutex_(Mutex::mutex()),
      should_do_colors_(true),
      formatter_(std::make_unique<details::PatternFormatter>(
          details::os::default_eol)) {
    colors_[level::k_trace] = white;
    colors_[level::k_debug] = cyan;
    colors_[level::k_info] = green;
    colors_[level::k_warn] = yellow_bold;
    colors_[level::k_err] = red_bold;
    colors_[level::k_critical] = bold_on_red;
    colors_[level::k_off] = reset;
}

// setColor(level::k_info, AnsicolorSink<Mutex>::red)
template <typename Mutex>
inline void AnsicolorSink<Mutex>::setColor(level::level_enum color_level,
                                           string_view_t color) {
    std::lock_guard<mutex_t> lock(mutex_);
    colors_[color_level] = color;
}

template <typename Mutex>
inline bool AnsicolorSink<Mutex>::shouldColor() {
    return should_do_colors_;
}

template <typename Mutex>
inline void AnsicolorSink<Mutex>::setShouldColor(bool should_color) {
    should_do_colors_ = should_color;
}

template <typename Mutex>
inline void AnsicolorSink<Mutex>::printColorCode(
    const string_view_t &color_code) {
    fwrite(color_code.data(), sizeof(char), color_code.size(), target_file_);
}

template <typename Mutex>
inline void AnsicolorSink<Mutex>::printRange(const memory_buf_t &formatted,
                                             size_t start, size_t end) {
    fwrite(formatted.data() + start, sizeof(char), end - start, target_file_);
}

template <typename Mutex>
inline void AnsicolorSink<Mutex>::flush() {
    std::lock_guard<mutex_t> lock(mutex_);
    fflush(target_file_);
}

template <typename Mutex>
inline void AnsicolorSink<Mutex>::log(const details::LogMessage &msg) {
    std::lock_guard<mutex_t> lock(mutex_);

    memory_buf_t formatted;
    formatter_->format(msg, formatted);
    if (should_do_colors_ && msg.color_range_end > msg.color_range_start) {
        // printf color code like "\033[37m"(level)"\033[m"
        printRange(formatted, 0, msg.color_range_start);
        printColorCode(colors_[msg.level]);
        printRange(formatted, msg.color_range_start, msg.color_range_end);
        printColorCode(reset);
        printRange(formatted, msg.color_range_end, formatted.size());
    } else {
        printRange(formatted, 0, formatted.size());
    }
    fflush(target_file_);
}

}  // namespace sinks

}  // namespace Jlog
