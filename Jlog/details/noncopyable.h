#pragma once

namespace Jlog {

namespace details {

class noncopyable {
protected:
    noncopyable() = default;
    ~noncopyable() = default;

private:
    noncopyable(const noncopyable&) = delete;
    const noncopyable& operator=(const noncopyable&) = delete;
};

}  // namespace details

}  // namespace Jlog