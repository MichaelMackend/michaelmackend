#ifndef MEMORYALLOCATOR_H
#define MEMORYALLOCATOR_H
#include <utility>
#include <map>
#include <stdexcept>
#include <vector>
#include "mallocator11.h"
#include "poolallocator11.h"

using byte = u_char;

class BlockAllocator;
class BackTrace;
struct AllocStack;
struct BlockHeader;
struct PageListHeader;
struct BlockAllocationRecord;

AllocStack backtrace();

class MemoryAllocator
{
public:
    MemoryAllocator();
    ~MemoryAllocator();

    static void Initialize(std::size_t memory_budget);
    void* AllocateBlockPage(std::size_t size);
    void FreeBlockPage(void* p, std::size_t size);

private:
    void InitializeWithMemoryBudget(std::size_t memory_budget);
    void InitializeBlockAllocators();
    void InitializeMemoryPool(std::size_t memory_budget);
    PageListHeader* FindMemoryBlockPageForSize(size_t size, PageListHeader **outPrevPage);
    PageListHeader* FindPrevMemoryBlockPageLocationForAddress(void* p);
    bool AddressIsInMemoryPool(void* p) const;
    bool AddressIsBlockPageAligned(void* p) const;
    friend void *operator new(size_t t);
    friend void operator delete(void* p) noexcept;
    void PrintAddressAllocCallStack(void* p);
    void* Alloc(size_t size);
    void Free(void* p);

    BlockAllocator *mBlockAllocators;
    std::size_t* mBlockSizeLookupTable;
    std::size_t mMaxBlockSize;
    
    std::size_t mTotalMemoryBudget;
    byte* mAllocatedPool;
    byte* mMemoryPool;
    PageListHeader* mFreeMemoryList;
    
    typedef std::map<std::size_t,
                         BackTrace,
                         std::less<std::size_t>,
                         Mallocator11<std::pair<const std::size_t, BackTrace>>>
            BackTraceMap;

    BackTraceMap mAllocatedPtrBackTraceMap;

    static const unsigned int ALLOC_TABLE_HASH_SIZE = 104729;

    typedef std::map<std::size_t, 
                    BlockAllocator*, 
                    std::less<std::size_t>, 
                    PoolAllocator11<std::pair<const std::size_t, BlockAllocator*> > > BlockAllocationMap;

    BlockAllocationMap* mAllocationRecords;
};

#endif // MEMORYALLOCATOR_H
