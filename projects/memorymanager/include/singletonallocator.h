#ifndef SINGLETON_ALLOCATOR_H
#define SINGLETON_ALLOCATOR_H

#include "memoryallocator.h"

class SingletonAllocator
{
public:
    static memtek::memory::MemoryAllocator& instance();
};


#endif