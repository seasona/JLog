#pragma once

#include <utility>

namespace Jlog {

namespace details {

/// use null_mutex can control some template class which use mutex in
/// multi-threads has better porfarmance in single thread 
struct NullMutex {
    void lock() const {}
    void unlock() const {}
    bool try_lock() const { return true; }
};

}  // namespace details

}  // namespace Jlog