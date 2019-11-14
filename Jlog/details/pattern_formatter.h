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
    explicit PatternFormatter(std::string eol) : eol_(eol) {}
    void format(const LogMessage &msg, memory_buf_t &dest) override;

private:
    std::string eol_;
    std::tm cached_tm_;
    std::chrono::seconds last_log_secs_;
    std::vector<std::unique_ptr<FlagFormatter>> formatters_;
};

/// override can't place in the defination of function, it should place in the
/// declaration of function
void PatternFormatter::format(const LogMessage &msg, memory_buf_t &dest) {
    auto secs = std::chrono::duration_cast<std::chrono::seconds>(
        msg.time.time_since_epoch());
    if (secs != last_log_secs_) {
        cached_tm_ = localTime(log_clock::to_time_t(msg.time));
        last_log_secs_ = secs;
    }
    for (auto &f : formatters_) {
        f->format(msg, cached_tm_, dest);
    }
    fmt_helper::appendStringView(eol_, dest);
}

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

            fmt_helper::pad2(tm_time.tm_sec, cached_datetime_);
            cached_datetime_.push_back('.');

            cache_timestamp_ = secs;
        }
        dest.append(cached_datetime_.begin(), cached_datetime_.end());

        // push back the millisecond
        auto millis = fmt_helper::timeFraction<milliseconds>(msg.time);
        
    }

private:
    std::chrono::seconds cache_timestamp_{0};
    memory_buf_t cached_datetime_;
};

}  // namespace details

}  // namespace Jlog