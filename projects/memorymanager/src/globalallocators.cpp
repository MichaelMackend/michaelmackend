#include "singletonallocator.h"

void *operator new(size_t size)
{
    void *p = SingletonAllocator::instance().Alloc(size);
    return p;
}

void operator delete(void *p) noexcept
{
    SingletonAllocator::instance().Free(p);
}