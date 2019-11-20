#pragma once

#include "Jlog/common.h"
#include "Jlog/fmt/fmt.h"

namespace Jlog {

namespace details {

namespace fmt_helper {

// append non-null string to the rear of memory_buffer dest
inline void appendStringView(string_view_t view, memory_buf_t &dest) {
    auto buf_ptr = view.data();
    if (buf_ptr != nullptr) {
        dest.append(buf_ptr, buf_ptr + view.size());
    }
}

template <typename T>
inline void appendInt(T n, memory_buf_t &dest) {
    fmt::format_int i(n);
    dest.append(i.data(), i.data() + i.size());
}

// pad n means if your number digits is smaller than n, then add zero before it
inline void pad2(int n, memory_buf_t &dest) {
    if (n > 99) {
        appendInt(n, dest);
    } else if (n > 9) {
        dest.push_back(static_cast<char>('0' + n / 10));
        dest.push_back(static_cast<char>('0' + n % 10));
    } else if (n >= 0) {
        // add one zero before it
        dest.push_back('0');
        dest.push_back(static_cast<char>('0' + n));
    } else {
        // let fmt to deal with negative numberF
        fmt::format_to(dest, ":02", n);
    }
}

// get the millisecond default
template <typename ToDuration>
inline ToDuration timeFraction(log_clock::time_point tp) {
    using std::chrono::duration_cast;
    using std::chrono::seconds;
    auto duration = tp.time_since_epoch();
    auto secs = duration_cast<seconds>(duration);
    return duration_cast<ToDuration>(duration) -
           duration_cast<ToDuration>(secs);
}

/// Bit Twiddling Hacks by Sean Eron Anderson fast get digits of unsigned number
/// [link](http://graphics.stanford.edu/~seander/bithacks.html#IntegerLog10)
/// [benchmark](https://github.com/localvoid/cxx-benchmark-count-digits)
template <typename T>
inline unsigned countDigits(T n) {
    /// std::conditional(bool, type1, type2)
    /// means the type = bool?type1:type2
    using count_type = typename std::conditional<(sizeof(T) > sizeof(uint32_t)),
                                                 uint64_t, uint32_t>::type;
    // T can be 8bit or 16bit, so must transform to 32bit
    return static_cast<unsigned>(
        fmt::internal::count_digits(static_cast<count_type>(n)));
}

// insert (width - digit) zeros before the unsigned, like: 003
template <typename T>
inline void padUnit(T n, unsigned int width, memory_buf_t &dest) {
    static_assert(std::is_unsigned<T>::value, "pad_uint must get unsigned T");
    auto digits = countDigits(n);
    if (width > digits) {
        const char *zeroes = "0000000000000000000";
        dest.append(zeroes, zeroes + width - digits);
    }
    appendInt(n, dest);
}

template <typename T>
inline void pad3(T n, memory_buf_t &dest) {
    padUnit(n, 3, dest);
}

}  // namespace fmt_helper

}  // namespace details

}  // namespace Jlog