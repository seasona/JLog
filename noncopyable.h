#pragma once

/**
 * @brief 将复制构造函数和复制赋值函数设为private，可使子类不可复制
 *
 */
class noncopyable {
protected:
    noncopyable() = default;
    ~noncopyable() = default;

private:
    noncopyable(const noncopyable&) = delete;
    const noncopyable& operator=(const noncopyable&) = delete;
};
