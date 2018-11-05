#ifndef MEMORYALLOCATOR_H
#define MEMORYALLOCATOR_H
#include <utility>
#include <map>
#include <stdexcept>
#include <vector>
#include "mallocator11.h"


class BlockAllocator;
class BackTrace;
struct AllocStack;
struct BlockHeader;
struct PageHeader;
struct BlockAllocationRecord;

AllocStack backtrace();

class MemoryAllocator
{
public:
    MemoryAllocator();
    ~MemoryAllocator();

    static void Initialize();

private:
    friend void* operator new(size_t t);
    friend void operator delete(void* p) noexcept;
    void PrintAddressAllocCallStack(void* p);
    void* Alloc(size_t size);
    void Free(void* p);
    BlockAllocator *mBlockAllocators;
    std::size_t* mBlockSizeLookupTable;
    std::size_t mMaxBlockSize;
    std::map<std::size_t, BackTrace, std::less<std::size_t>, Mallocator11<std::pair<const std::size_t, BackTrace> > > mAllocatedPtrBackTraceMap;
    static const unsigned int ALLOC_TABLE_HASH_SIZE = 104729;
    typedef std::vector<BlockAllocationRecord, Mallocator11<BlockAllocationRecord>> BlockAllocationVector;
    BlockAllocationVector* mAllocationRecords;//[ALLOC_TABLE_HASH_SIZE];
};

#endif // MEMORYALLOCATOR_H
