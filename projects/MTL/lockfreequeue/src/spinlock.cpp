#include "spinlock.h"

namespace memtl {
    spinlock::spinlock() : mLock(ATOMIC_FLAG_INIT) {

    }

    spinlock::spinlock(const spinlock& lock) : mLock(ATOMIC_FLAG_INIT) {
    }

    void spinlock::lock() {
        while(mLock.test_and_set(std::memory_order_acquire));
    }

    void spinlock::unlock() {
        mLock.clear(std::memory_order_release);
    }

    spinlock_exchange::spinlock_exchange() : mLock(0) { }

    spinlock_exchange::spinlock_exchange(const spinlock_exchange&) : mLock(0) { }

    void spinlock_exchange::lock() {
       // while(mLock.exchange(1, std::memory_order_acquire) == 1);
        while(mLock.exchange(1) == 1);
    }

    void spinlock_exchange::unlock() {
        //mLock.store(0, std::memory_order_release);
        mLock.store(0);
    }
}