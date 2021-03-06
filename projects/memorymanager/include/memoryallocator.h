#ifndef MEMORYALLOCATOR_H
#define MEMORYALLOCATOR_H

#include <utility>
#include <map>
#include <stdexcept>

#if RECORD_CALLSTACKS
#include "mallocator11.h"
AllocStack backtrace();
#endif

namespace memtek
{
namespace memory
{

using byte = uint8_t;

class BlockAllocator;
class BackTrace;
struct AllocStack;
struct BlockHeader;
struct PageListHeader;
struct BlockAllocationRecord;

class MemoryAllocator
{
    friend class BlockAllocator;

  public:
    MemoryAllocator();
    ~MemoryAllocator();
    void Initialize(const std::size_t& pool_size);
    void PrintAllocationSummaryReport(char *buf);
    void CheckIntegrity();
    void *Alloc(size_t size);
    void Free(void *p);

  private:
    void InitializeWithMemoryBudget(std::size_t memory_budget);
    void *AllocateBlockPage(std::size_t size, BlockAllocator *ba);
    void FreeBlockPage(void *p, std::size_t size, BlockAllocator *ba);
    void InitializeBlockAllocators();
    void InitializeMemoryPool(std::size_t memory_budget);
    void InsertNewPageListHeaderBlock(PageListHeader *pageToAlloc, std::size_t requestedSize, PageListHeader *prevPage);
    void UnlinkEntirePageListHeaderBlock(PageListHeader *pageToAlloc, PageListHeader *prevPage);
    void RecordNewPageOwner(byte *addr, std::size_t size, u_char allocatorIndex);
    void RemovePageOwner(byte *addr, std::size_t size);

  private:
    void PrintAddressAllocCallStack(void *p) const;
    bool Initialized() const;
    std::size_t ComputePageSlotIndexForAddress(byte *addr) const;
    BlockAllocator *FindBlockAllocatorForAllocatedAddress(byte *addr) const;
    bool AddressIsInMemoryPool(void *p) const;
    PageListHeader *FindMemoryBlockPageForSize(size_t size, PageListHeader **outPrevPage) const;
    PageListHeader *FindPrevMemoryBlockPageLocationForAddress(void *p) const;

  private:
    BlockAllocator *mBlockAllocators;
    std::size_t *mBlockSizeLookupTable;
    std::size_t mMaxBlockSize;
    std::size_t mNumBlockSizes;

    std::size_t mTotalMemoryBudget;
    std::size_t mRemainingMemory;
    byte *mAllocatedPool;
    byte *mMemoryPool;
    PageListHeader *mFreePageHead;
    PageListHeader *mPageHeaderList;

    u_char *mAllocatorPageMap;
    std::size_t mNumPageMapSlots;
    std::size_t mPageSlotMemorySize;

#if RECORD_CALLSTACKS
    typedef std::map<std::size_t,
                     BackTrace,
                     std::less<std::size_t>,
                     Mallocator11<std::pair<const std::size_t, BackTrace>>>
        BackTraceMap;

    BackTraceMap mAllocatedPtrBackTraceMap;
#endif
};

} // namespace memory
} // namespace memtek

#endif // MEMORYALLOCATOR_H
