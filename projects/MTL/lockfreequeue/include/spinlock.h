#ifndef MEMTL_SPINLOCK_H
#define MEMTL_SPINLOCK_H

#include <atomic>

namespace memtl
{
    class spinlock
    {
        public:
        spinlock();
        spinlock(const spinlock&);
        void lock();
        void unlock();
        private:
        std::atomic_flag mLock;
    };

    class spinlock_exchange 
    {
        public:
        spinlock_exchange();
        spinlock_exchange(const spinlock_exchange&);
        void lock();
        void unlock();
        private:
        std::atomic_int mLock;
    };
}


#endif //MEMTL_SPINLOCK_H