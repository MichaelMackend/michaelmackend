#ifndef MEMORYALLOCATOR_H
#define MEMORYALLOCATOR_H
#include <utility>
#include <map>
#include <stdexcept>
#include <vector>
#include "poolallocator11.h"

#if RECORD_CALLSTACKS
#include "mallocator11.h"
#endif

using byte = uint8_t;

class BlockAllocator;
class BackTrace;
struct AllocStack;
struct BlockHeader;
struct PageListHeader;
struct BlockAllocationRecord;

AllocStack backtrace();

class MemoryAllocator
{
    friend class BlockAllocator;
    
public:
    MemoryAllocator();
    ~MemoryAllocator();

    static void Initialize(std::size_t memory_budget);
    void* AllocateBlockPage(std::size_t size, BlockAllocator* ba);
    void FreeBlockPage(void* p, std::size_t size, BlockAllocator* ba);
    static void PrintAllocationSummaryReport(char* buf);
    static void CheckIntegrity();

  private:
    void InitializeWithMemoryBudget(std::size_t memory_budget);
    void InitializeBlockAllocators();
    void InitializeMemoryPool(std::size_t memory_budget);
    PageListHeader* FindMemoryBlockPageForSize(size_t size, PageListHeader **outPrevPage);
    PageListHeader* FindPrevMemoryBlockPageLocationForAddress(void* p);
    bool PageIsAdjacentToPreviousPage(PageListHeader* page, PageListHeader* prevPage) const;
    bool TryJoinPages(PageListHeader* startPage, PageListHeader* pageToAppend) const;
    bool AddressIsInMemoryPool(void* p) const;
    bool AddressIsBlockPageAligned(void* p) const;
    bool AllocatedPageHasEnoughSpaceForNewPageListHeaderBlock(PageListHeader* pageToAlloc, std::size_t requestedSize);
    void InsertNewPageListHeaderBlock(PageListHeader* pageToAlloc, std::size_t requestedSize, PageListHeader* prevPage);
    void UnlinkEntirePageListHeaderBlock(PageListHeader* pageToAlloc, PageListHeader* prevPage);
    static std::size_t GetBlockPageAlignedSize(std::size_t size);
    static byte* GetBlockPageAlignedAddress(byte* addr);
    std::size_t ComputePageSlotIndexForAddress(byte* addr);
    BlockAllocator* FindBlockAllocatorForAllocatedAddress(byte* addr);
    void RecordNewPageOwner(byte* addr, std::size_t size, u_char allocatorIndex);
    void RemovePageOwner(byte* addr, std::size_t size);

    friend void *operator new(size_t t);
    friend void operator delete(void* p) noexcept;
    void PrintAddressAllocCallStack(void* p);
    void* Alloc(size_t size);
    void Free(void* p);
    bool Initialized();

    BlockAllocator *mBlockAllocators;
    std::size_t* mBlockSizeLookupTable;
    std::size_t mMaxBlockSize;
    std::size_t mNumBlockSizes;
    
    std::size_t mTotalMemoryBudget;
    std::size_t mRemainingMemory;
    byte* mAllocatedPool;
    byte* mMemoryPool;
    PageListHeader* mFreePageHead;
    PageListHeader* mPageHeaderList;
 
#if RECORD_CALLSTACKS
    typedef std::map<std::size_t,
                         BackTrace,
                         std::less<std::size_t>,
                         Mallocator11<std::pair<const std::size_t, BackTrace>>>
            BackTraceMap;


    BackTraceMap mAllocatedPtrBackTraceMap;
#endif

    static const unsigned int ALLOC_TABLE_HASH_SIZE = 104729;

    u_char* mAllocatorPageMap;
    std::size_t mNumPageMapSlots;
    std::size_t mPageSlotMemorySize;
    
};

#endif // MEMORYALLOCATOR_H
