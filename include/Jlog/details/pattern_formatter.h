#pragma once
#include <memory>
#include <vector>
#include "Jlog/details/fmt_helper.h"
#include "Jlog/formatter.h"

namespace Jlog {

namespace details {

class FlagFormatter {
public:
    FlagFormatter() = default;
    ~FlagFormatter() = default;
    virtual void format(const LogMessage &msg, const std::tm &time,
                        memory_buf_t &dest) = 0;
};

class PatternFormatter final : public Formatter {
public:
    explicit PatternFormatter(std::string eol);
    void format(const LogMessage &msg, memory_buf_t &dest) override;

private:
    std::string eol_;
    std::tm cached_tm_;
    std::chrono::seconds last_log_secs_;
    std::vector<std::unique_ptr<FlagFormatter>> formatters_;
};

// format the filename
class FilenameFormatter final : public FlagFormatter {
public:
    explicit FilenameFormatter() = default;

    /// std::strrchr(const char* str, int ch);
    /// find the last ch occur location, like the ~/home/log will return the
    /// location of log
    static const char *basename(const char *filename) {
        const char *rv = std::strrchr(filename, os::default_folder_sep);
        return rv != nullptr ? rv + 1 : filename;
    }

    void format(const details::LogMessage &msg, const std::tm &time,
                memory_buf_t &dest) override {
        if (msg.source.empty()) {
            return;
        }
        auto filename = basename(msg.source.filename);
        fmt_helper::appendStringView(filename, dest);
    }
};

// default formatter, format like:
// [%Y-%m-%d %H:%M:%S.%e] [%n] [%l] %v
class FullFormatter final : public FlagFormatter {
public:
    explicit FullFormatter() = default;
    ~FullFormatter() = default;

    void format(const details::LogMessage &msg, const std::tm &tm_time,
                memory_buf_t &dest) override {
        using std::chrono::duration_cast;
        using std::chrono::milliseconds;
        using std::chrono::seconds;

        // time_since_epoch count
        auto duration = msg.time.time_since_epoch();
        auto secs = duration_cast<seconds>(duration);

        // cache the time except millisecond, so every time log just millisecond
        // don't need to push back time again and can just use the cached memory
        // before
        if (cache_timestamp_ != secs || cached_datetime_.size() == 0) {
            cached_datetime_.clear();
            cached_datetime_.push_back('[');
            fmt_helper::appendInt(tm_time.tm_year + 1900, cached_datetime_);
            cached_datetime_.push_back('-');

            fmt_helper::pad2(tm_time.tm_mon + 1, cached_datetime_);
            cached_datetime_.push_back('-');

            fmt_helper::pad2(tm_time.tm_mday, cached_datetime_);
            cached_datetime_.push_back(' ');

            fmt_helper::pad2(tm_time.tm_hour, cached_datetime_);
            cached_datetime_.push_back(':');

            fmt_helper::pad2(tm_time.tm_min, cached_datetime_);
            cached_datetime_.push_back(':');

            fmt_helper::pad2(tm_time.tm_sec, cached_datetime_);
            cached_datetime_.push_back('.');

            cache_timestamp_ = secs;
        }
        dest.append(cached_datetime_.begin(), cached_datetime_.end());

        // push back the millisecond
        auto millis = fmt_helper::timeFraction<milliseconds>(msg.time);
        fmt_helper::pad3(static_cast<uint32_t>(millis.count()), dest);
        dest.push_back(']');
        dest.push_back(' ');

        // push back the logger name
        if (msg.logger_name.size() > 0) {
            dest.push_back('[');
            fmt_helper::appendStringView(msg.logger_name, dest);
            dest.push_back(']');
            dest.push_back(' ');
        }

        // push back the logger level
        dest.push_back('[');
        msg.color_range_start = dest.size();
        fmt_helper::appendStringView(level::toStringView(msg.level), dest);
        msg.color_range_end = dest.size();
        dest.push_back(']');
        dest.push_back(' ');

        // push back the file name and line
        if (!msg.source.empty()) {
            dest.push_back('[');
            const char *filename = FilenameFormatter::basename(msg.source.filename);
            fmt_helper::appendStringView(filename, dest);
            dest.push_back(':');
            fmt_helper::appendInt(msg.source.line, dest);
            dest.push_back(']');
            dest.push_back(' ');
        }

        // push back the log message
        fmt_helper::appendStringView(msg.payload, dest);
    }

private:
    std::chrono::seconds cache_timestamp_{0};
    memory_buf_t cached_datetime_;
};


PatternFormatter::PatternFormatter(std::string eol): eol_(eol){
    formatters_.push_back(std::make_unique<FullFormatter>());
}

/// override can't place in the defination of function, it should place in the
/// declaration of function
void PatternFormatter::format(const LogMessage &msg, memory_buf_t &dest) {
    auto secs = std::chrono::duration_cast<std::chrono::seconds>(
        msg.time.time_since_epoch());
    if (secs != last_log_secs_) {
        cached_tm_ = os::localTime(log_clock::to_time_t(msg.time));
        last_log_secs_ = secs;
    }
    for (auto &f : formatters_) {
        f->format(msg, cached_tm_, dest);
    }
    fmt_helper::appendStringView(eol_, dest);
}


}  // namespace details

}  // namespace Jlog