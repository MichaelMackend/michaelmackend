#include "singletonallocator.h"


memtek::memory::MemoryAllocator& SingletonAllocator::instance() {
    static memtek::memory::MemoryAllocator allocator;
    return allocator;
}