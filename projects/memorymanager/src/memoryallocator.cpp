#include <iostream>
#include "backtrace.h"

#include <utility>
#include <map>
#include <stdexcept>
#include <algorithm>
#include <cstddef>
#include <cstdlib>
#include "memoryallocator.h"
#include <stdio.h>
#include <thread>
#include <mutex>
#include <assert.h>
#include <cstring>
#include <cstdint>
#include <limits.h>


namespace memtek
{
namespace memory
{

using byte = uint8_t;

const unsigned long BYTE = 1;
const unsigned long KILOBYTE = 1024;
const unsigned long MEGABYTE = 1024 * 1024;
const unsigned long GIGABYTE = 1024 * 1024 * 1024;
const unsigned long LARGE_PRIME = 104729;

const std::size_t MEMORY_POOL_ALIGNMENT = 64;
const std::size_t BLOCK_PAGE_ALIGNMENT = 64;
const std::size_t BLOCKS_PER_PAGE = 64;

size_t GetBlockPageAlignedSize(size_t size);
byte *GetBlockPageAlignedAddress(byte *addr);
bool AllocatedPageHasEnoughSpaceForNewPageListHeaderBlock(PageListHeader *pageToAlloc, std::size_t requestedSize);
bool AddressIsBlockPageAligned(void *p);
bool PageIsAdjacentToPreviousPage(PageListHeader *page, PageListHeader *prevPage);
bool TryJoinPages(PageListHeader *startPage, PageListHeader *pageToAppend);

struct PageHeader
{
    PageHeader()
        : mFreeBlocks(0), mPrevPage(nullptr), mNextPage(nullptr)
    {
    }
    uint64_t mFreeBlocks;
    PageHeader *mNextPage;
    PageHeader *mPrevPage;
    byte *blockHead()
    {
        return reinterpret_cast<byte *>(this + 1);
        ;
    }
};

class PageListHeader
{
  private:
    PageListHeader *mNextPage;

  public:
    PageListHeader()
        : mNextPage(nullptr), mPageSize(0)
    {
    }

    void SetNextPage(PageListHeader *ph)
    {
        assert(ph != this);
        this->mNextPage = ph;
    }
    PageListHeader *NextPage()
    {
        return mNextPage;
    }
    std::size_t mPageSize;
};

class PageTable
{
  public:
    PageTable()
        : mPageTable0(nullptr), mPageTable1(nullptr), mNumPageSlots(0), mMemoryAllocatorPoolStartAddress(nullptr), mNominalPageSize(0)
    {
    }

    ~PageTable()
    {
        free(mPageTable0);
        free(mPageTable1);
    }

    void InitializePageTable(size_t numPageSlots, size_t nominalPageSize, byte *memoryAllocatorPoolStartAddress)
    {
        mNominalPageSize = nominalPageSize;
        mMemoryAllocatorPoolStartAddress = memoryAllocatorPoolStartAddress;
        const size_t pageTableSize = sizeof(PageHeader *) * numPageSlots;
        mPageTable0 = (PageHeader **)malloc(pageTableSize);
        mPageTable1 = (PageHeader **)malloc(pageTableSize);
        for (int i = 0; i < numPageSlots; ++i)
        {
            mPageTable0[i] = nullptr;
            mPageTable1[i] = nullptr;
        }
    }
    size_t ComputeBlockPageIndexForAddress(void *addr);
    void RecordPageInPageTable(PageHeader *page);
    void RemovePageFromPageTable(PageHeader *page);
    PageHeader *GetPageForAddress(void *addr)
    {
        size_t index = ComputeBlockPageIndexForAddress(addr);
        PageHeader **pageTable = mPageTable0[index] && PageOwnsAddr(mPageTable0[index], addr)
                                     ? mPageTable0
                                     : mPageTable1[index] && PageOwnsAddr(mPageTable1[index], addr)
                                           ? mPageTable1
                                           : nullptr;
        if (pageTable == nullptr)
        {
            return nullptr;
        }
        return pageTable[index];
    }
    bool PageOwnsAddr(PageHeader *ph, void *addr)
    {
        return addr >= ph->blockHead() && addr < ((byte *)ph + mNominalPageSize);
    }

  private:
    PageHeader **mPageTable0;
    PageHeader **mPageTable1;
    size_t mNumPageSlots;
    size_t mNominalPageSize;
    byte *mMemoryAllocatorPoolStartAddress;
};

class BlockAllocator
{
    friend class MemoryAllocator;

  public:
    BlockAllocator(
        MemoryAllocator *memAllocator,
        size_t indexInMemoryAllocator,
        size_t block_size = 64,
        size_t page_maxsize = MEGABYTE);

    ~BlockAllocator();

    void *Alloc();
    void Free(void *p);

  private:
    void PushNewPageAsHead(PageHeader *page);

    void AllocateNewPage();
    void InitializePageBlocks(PageHeader *page);
    void UnlinkPage(PageHeader *pageToUnlink);
    void AppendPage(PageHeader *PageToAppend);
    size_t GetBlockIndexForAddress(void *addr, PageHeader *&outPageHeader);

    MemoryAllocator *mMemoryAllocator;
    PageHeader *mPageHead;
    PageHeader *mPageTail;
    byte *mMemoryAllocatorPoolStartAddress;
    PageTable mPageTable;
    const size_t mBlockSize;
    const size_t mPageSize;
    const size_t mAllocatedPageSize;
    const size_t mBlocksPerPage;
    const size_t mIndexInMemoryAllocator;
};

struct BlockAllocationRecord
{
    BlockAllocationRecord(void *addr, BlockAllocator *ba) : mAddr(addr), mBlockAllocator(ba) {}
    void *mAddr;
    BlockAllocator *mBlockAllocator;
};

// sorted from smallest to largest
static const size_t kBlockSizes[] =
    {
        // 4-increments
        4, 8, 12, 16, 20, 24, 28, 32, 36, 40, 44, 48,
        52, 56, 60, 64, 68, 72, 76, 80, 84, 88, 92, 96,

        // 32-increments
        128, 160, 192, 224, 256, 288, 320, 352, 384,
        416, 448, 480, 512, 544, 576, 608, 640,

        // 64-increments
        704, 768, 832, 896, 960, 1024
        
    };

void MemoryAllocator::InitializeWithMemoryBudget(std::size_t memory_budget)
{
    InitializeMemoryPool(memory_budget);
    InitializeBlockAllocators();
}

void MemoryAllocator::InitializeMemoryPool(std::size_t memory_budget)
{
    mTotalMemoryBudget = memory_budget;
    mRemainingMemory = mTotalMemoryBudget;

    mAllocatedPool = reinterpret_cast<byte *>(malloc(mTotalMemoryBudget + MEMORY_POOL_ALIGNMENT));
    mMemoryPool = GetBlockPageAlignedAddress(mAllocatedPool);
    assert(AddressIsBlockPageAligned(mMemoryPool));
    mFreePageHead = (PageListHeader *)mMemoryPool;

    mFreePageHead->SetNextPage(nullptr);
    mFreePageHead->mPageSize = mTotalMemoryBudget;
}

void MemoryAllocator::InitializeBlockAllocators()
{
    mNumBlockSizes = sizeof(kBlockSizes) / sizeof(kBlockSizes[0]);
    const size_t maxBlockSize = kBlockSizes[mNumBlockSizes - 1];

    mBlockAllocators = reinterpret_cast<BlockAllocator *>(malloc(mNumBlockSizes * sizeof(BlockAllocator)));
    for (int i = 0; i < mNumBlockSizes; ++i)
    {
        new (mBlockAllocators + i) BlockAllocator(this, i, kBlockSizes[i]);
    }

    mBlockSizeLookupTable = reinterpret_cast<size_t *>(malloc((maxBlockSize + 1) * sizeof(size_t)));
    size_t blockSizeIndex = 0;
    for (size_t i = 0; i < maxBlockSize; ++i)
    {
        if (i > kBlockSizes[blockSizeIndex])
        {
            blockSizeIndex++;
        }
        mBlockSizeLookupTable[i] = blockSizeIndex;
    }

    mPageSlotMemorySize = BLOCK_PAGE_ALIGNMENT;
    mNumPageMapSlots = mTotalMemoryBudget / mPageSlotMemorySize;
    mAllocatorPageMap = (u_char *)malloc(mNumPageMapSlots);
    memset((void *)mAllocatorPageMap, UCHAR_MAX, mNumPageMapSlots);

    mMaxBlockSize = maxBlockSize;
}

//O(N) search. todo: consider upgrading to a tree based structure if possible
PageListHeader *MemoryAllocator::FindMemoryBlockPageForSize(size_t size, PageListHeader **outPrevPage) const
{
    PageListHeader *prev = nullptr;
    PageListHeader *ph = mFreePageHead;
    const size_t sizeNeeded = size + sizeof(PageListHeader);
    while (ph != nullptr)
    {
        if (ph->mPageSize >= sizeNeeded)
        {
            if (outPrevPage != nullptr)
            {
                *outPrevPage = prev;
            }
            return ph;
        }
        prev = ph;
        ph = ph->NextPage();
    }
    throw "insufficient memory for request";
    return nullptr;
}

void *MemoryAllocator::AllocateBlockPage(std::size_t requestedSize, BlockAllocator *ba)
{

    requestedSize = GetBlockPageAlignedSize(requestedSize);

    PageListHeader *prevPage = nullptr;

    PageListHeader *pageToAlloc = FindMemoryBlockPageForSize(requestedSize, &prevPage);

    assert(AddressIsBlockPageAligned(pageToAlloc));

    RecordNewPageOwner(reinterpret_cast<byte *>(pageToAlloc), requestedSize, ba->mIndexInMemoryAllocator);

    InsertNewPageListHeaderBlock(pageToAlloc, requestedSize, prevPage);

    /*char logBuf[512] = {'\0'};
    PrintAllocationSummaryReport(logBuf);
    std::cout << "Alloc Summary: " << logBuf << std::endl;*/

    mRemainingMemory -= requestedSize;

    return pageToAlloc;
}

//[62]

size_t GetBlockPageAlignedSize(size_t size)
{
    std::size_t blockAlignmentPadding = (BLOCK_PAGE_ALIGNMENT - (size % BLOCK_PAGE_ALIGNMENT)) % BLOCK_PAGE_ALIGNMENT;
    return size + blockAlignmentPadding;
}

byte *GetBlockPageAlignedAddress(byte *addr)
{
    std::size_t addrVal = reinterpret_cast<std::size_t>(addr);
    std::size_t blockAlignmentPadding = (BLOCK_PAGE_ALIGNMENT - (addrVal % BLOCK_PAGE_ALIGNMENT)) % BLOCK_PAGE_ALIGNMENT;
    return addr + blockAlignmentPadding;
}

size_t MemoryAllocator::ComputePageSlotIndexForAddress(byte *addr) const
{
    ptrdiff_t addrOffset = addr - mMemoryPool;
    size_t pageSlotIndex = addrOffset / mPageSlotMemorySize;
    return pageSlotIndex;
}

BlockAllocator *MemoryAllocator::FindBlockAllocatorForAllocatedAddress(byte *addr) const
{
    size_t pageSlotIndex = ComputePageSlotIndexForAddress(addr);
    u_char blockAllocatorIndex = mAllocatorPageMap[pageSlotIndex];
    if (blockAllocatorIndex >= mNumBlockSizes)
    {
        return nullptr;
    }
    BlockAllocator *ba = &(mBlockAllocators[blockAllocatorIndex]);
    return ba;
}

void MemoryAllocator::RecordNewPageOwner(byte *addr, std::size_t size, u_char allocatorIndex)
{
    size_t pageSlotIndexStart = ComputePageSlotIndexForAddress(addr);
    size_t pageSlotIndexEnd = ComputePageSlotIndexForAddress(addr + size);
    memset(mAllocatorPageMap + pageSlotIndexStart, allocatorIndex, pageSlotIndexEnd - pageSlotIndexStart);
}

void MemoryAllocator::RemovePageOwner(byte *addr, std::size_t size)
{
    return RecordNewPageOwner(addr, size, std::numeric_limits<u_char>::max());
}

bool AllocatedPageHasEnoughSpaceForNewPageListHeaderBlock(PageListHeader *pageToAlloc, std::size_t requestedSize)
{
    return (pageToAlloc->mPageSize - requestedSize) >= sizeof(PageListHeader);
}

void MemoryAllocator::InsertNewPageListHeaderBlock(PageListHeader *pageToAlloc, std::size_t requestedSize, PageListHeader *prevPage)
{
    PageListHeader *newFreeBlock = new (reinterpret_cast<byte *>(pageToAlloc) + requestedSize) PageListHeader;
    assert(AddressIsInMemoryPool(newFreeBlock));
    newFreeBlock->SetNextPage(pageToAlloc->NextPage());
    newFreeBlock->mPageSize = pageToAlloc->mPageSize - requestedSize;

    assert(prevPage != nullptr || mFreePageHead == pageToAlloc);

    if (prevPage != nullptr)
    {
        prevPage->SetNextPage(newFreeBlock);
    }
    else
    {
        mFreePageHead = newFreeBlock;
    }
}
void MemoryAllocator::UnlinkEntirePageListHeaderBlock(PageListHeader *pageToAlloc, PageListHeader *prevPage)
{

    assert(prevPage != nullptr || mFreePageHead == pageToAlloc);

    if (prevPage != nullptr)
    {
        prevPage->SetNextPage(pageToAlloc->NextPage());
    }
    else
    {
        mFreePageHead = pageToAlloc->NextPage();
    }
}

bool MemoryAllocator::AddressIsInMemoryPool(void *p) const
{
    return p >= mMemoryPool && (p < (mMemoryPool + mTotalMemoryBudget));
}

bool AddressIsBlockPageAligned(void *p)
{
    return (reinterpret_cast<std::size_t>(p) % BLOCK_PAGE_ALIGNMENT) == 0;
}

PageListHeader *MemoryAllocator::FindPrevMemoryBlockPageLocationForAddress(void *p) const
{
    if (!AddressIsInMemoryPool(p))
    {
        return nullptr;
    }

    PageListHeader *prev = nullptr;
    PageListHeader *ph = mFreePageHead;
    while (ph != nullptr)
    {
        if (ph > p)
        {
            return prev;
        }
        prev = ph;
        ph = ph->NextPage();
    }
    return nullptr;
}

bool PageIsAdjacentToPreviousPage(PageListHeader *page, PageListHeader *prevPage)
{
    if (page == nullptr || prevPage == nullptr)
    {
        return false;
    }

    return (reinterpret_cast<byte *>(prevPage) + prevPage->mPageSize) == reinterpret_cast<byte *>(page);
}

bool TryJoinPages(PageListHeader *startPage, PageListHeader *pageToAppend)
{
    if (PageIsAdjacentToPreviousPage(pageToAppend, startPage))
    {
        startPage->mPageSize += pageToAppend->mPageSize;
        startPage->SetNextPage(pageToAppend->NextPage());
        return TryJoinPages(startPage, startPage->NextPage());
    }
    return false;
}

void MemoryAllocator::FreeBlockPage(void *p, std::size_t requestedSize, BlockAllocator *ba)
{
    if (!AddressIsInMemoryPool(p))
    {
        throw "MemoryAllocator::FreeBlockPage received address out of pool range!";
    }

    if (!AddressIsBlockPageAligned(p))
    {
        throw "MemoryAllocator::FreeBlockPage received misaligned address!";
    }

    ///todo validate that the free address being freed is not already part of a free blockGetBlockPageAlignedSize
    requestedSize = GetBlockPageAlignedSize(requestedSize);

    PageListHeader *newReturnedPage = new (p) PageListHeader;
    newReturnedPage->mPageSize = requestedSize;

    PageListHeader *joinStart = nullptr;
    if (newReturnedPage < mFreePageHead)
    {
        newReturnedPage->SetNextPage(mFreePageHead);
        mFreePageHead = newReturnedPage;
        joinStart = newReturnedPage;
    }
    else
    {
        PageListHeader *prev = FindPrevMemoryBlockPageLocationForAddress(p);
        assert(newReturnedPage != prev->NextPage());
        newReturnedPage->SetNextPage(prev->NextPage());
        prev->SetNextPage(newReturnedPage);
        joinStart = prev;
    }

    RemovePageOwner(reinterpret_cast<byte *>(p), requestedSize);

    mRemainingMemory += requestedSize;

    TryJoinPages(joinStart, joinStart->NextPage());
}

void MemoryAllocator::PrintAllocationSummaryReport(char *buf)
{
    std::size_t freeMemoryTotal = 0;
    PageListHeader *ph = mFreePageHead;
    while (ph)
    {
        freeMemoryTotal += ph->mPageSize;
        ph = ph->NextPage();
    }
    long long remaining = mTotalMemoryBudget - freeMemoryTotal;
    //std::cout << "free page memory (" << 100.0 * (double)freeMemoryTotal / (double)().mTotalMemoryBudget << "%): " << freeMemoryTotal << "/" << ().mTotalMemoryBudget << "\n";
    sprintf(buf, "free page memory %lu bytes, free: %lu/%lu (%fpct)\n", remaining, freeMemoryTotal, mTotalMemoryBudget, 100.0 * (double)freeMemoryTotal / (double)mTotalMemoryBudget);
}

void MemoryAllocator::CheckIntegrity()
{
    int freeMemoryTotal = 0;
    PageListHeader *ph = mFreePageHead;
    while (ph)
    {
        freeMemoryTotal += ph->mPageSize;
        ph = ph->NextPage();
    }
    assert(freeMemoryTotal == mRemainingMemory);
}

MemoryAllocator::MemoryAllocator()
    : mBlockAllocators(nullptr), mBlockSizeLookupTable(nullptr), mAllocatedPool(nullptr), mTotalMemoryBudget(0)
{
}


MemoryAllocator::~MemoryAllocator()
{
    free(mBlockAllocators);
    free(mBlockSizeLookupTable);
    free(mAllocatedPool);
}

void MemoryAllocator::Initialize(const std::size_t& total_memory) {
    InitializeWithMemoryBudget(total_memory);
}

void *MemoryAllocator::Alloc(size_t size)
{
    if (!Initialized())
    {
        return malloc(size);
    }

    if (size == 0)
    {
        return nullptr;
    }

    if (size > mMaxBlockSize)
    {
        std::cout << "malloc!\n"
                  << std::flush;
        return malloc(size);
    }

    const size_t blockAllocatorIndex = mBlockSizeLookupTable[size];
    BlockAllocator *ba = mBlockAllocators + blockAllocatorIndex;
    void *block = ba->Alloc();

#if RECORD_CALLSTACKS
    mAllocatedPtrBackTraceMap[blockPtrAsValue] = BackTrace::CreateBackTrace();
#endif

    return block;
}

void MemoryAllocator::Free(void *p)
{
    if (!Initialized())
    {
        return free(p);
    }

    try
    {
        BlockAllocator *ba = FindBlockAllocatorForAllocatedAddress(reinterpret_cast<byte *>(p));
        if (ba != nullptr)
        {
            return ba->Free(p);
        }
        else
        {
            return free(p);
        }
    }
    catch (const char *msg)
    {
        std::cout << "MemoryAllocator::Free error for address: " << p << ": " << msg << std::endl;
        PrintAddressAllocCallStack(p);
        throw msg;
    }
}

bool MemoryAllocator::Initialized() const
{
    return mMemoryPool != nullptr;
}

void MemoryAllocator::PrintAddressAllocCallStack(void *p) const
{
#if RECORD_CALLSTACKS
    std::cout << "ADDRESS " << p << " ALLOCATED AT: \n";
    auto mapIter = mAllocatedPtrBackTraceMap.find(reinterpret_cast<std::size_t>(p));
    if (mapIter != mAllocatedPtrBackTraceMap.end())
    {
        mapIter->second.Print();
    }
    else
    {
        std::cout << "NOT FOUND." << std::endl;
    }
#endif
}

//[[ph][64*blocksize]]

BlockAllocator::BlockAllocator(MemoryAllocator *memAllocator, size_t indexInMemoryAllocator, size_t block_size, size_t page_maxSize)
    : mMemoryAllocator(memAllocator), mIndexInMemoryAllocator(indexInMemoryAllocator), mPageHead(nullptr), mPageTail(nullptr), mBlockSize(block_size), mPageSize(std::min(64 * block_size, page_maxSize)), mAllocatedPageSize(GetBlockPageAlignedSize(mPageSize + sizeof(PageHeader))), mBlocksPerPage(mPageSize / mBlockSize)
{
    mMemoryAllocatorPoolStartAddress = mMemoryAllocator->mMemoryPool;
    const size_t pageTableLength = mMemoryAllocator->mTotalMemoryBudget / mAllocatedPageSize;
    mPageTable.InitializePageTable(pageTableLength, mAllocatedPageSize, mMemoryAllocatorPoolStartAddress);
}

BlockAllocator::~BlockAllocator()
{
    //cleanup
}

void BlockAllocator::PushNewPageAsHead(PageHeader *newPage)
{
    newPage->mNextPage = mPageHead;
    newPage->mPrevPage = nullptr;
    if (mPageHead != nullptr)
    {
        mPageHead->mPrevPage = newPage;
    }
    else
    {
        mPageTail = newPage;
    }
    mPageHead = newPage;
}

void PageTable::RecordPageInPageTable(PageHeader *newPage)
{
    size_t startPageIndex = ComputeBlockPageIndexForAddress(newPage);
    size_t endPageIndex = ComputeBlockPageIndexForAddress(reinterpret_cast<byte *>(newPage) + (mNominalPageSize));

    PageHeader **pageTable = !mPageTable0[startPageIndex] ? mPageTable0 : !mPageTable1[startPageIndex] ? mPageTable1 : nullptr;
    assert(pageTable);
    pageTable[startPageIndex] = newPage;
    pageTable = !mPageTable0[endPageIndex] ? mPageTable0 : !mPageTable1[endPageIndex] ? mPageTable1 : nullptr;
    assert(pageTable);
    pageTable[endPageIndex] = newPage;
}

//576 + 3136 + 1600 + 1088 + 1088
// 0,0   0,1    2,3    4,5     5,6
//0     576    3712    5312    6400
//

void PageTable::RemovePageFromPageTable(PageHeader *page)
{
    size_t startPageIndex = ComputeBlockPageIndexForAddress(page);
    size_t endPageIndex = ComputeBlockPageIndexForAddress(reinterpret_cast<byte *>(page) + (mNominalPageSize));
    PageHeader **pageTable = mPageTable0[startPageIndex] == page ? mPageTable0 : mPageTable1[startPageIndex] == page ? mPageTable1 : nullptr;
    assert(pageTable);
    pageTable[startPageIndex] = nullptr;
    pageTable = mPageTable0[endPageIndex] == page ? mPageTable0 : mPageTable1[endPageIndex] == page ? mPageTable1 : nullptr;
    assert(pageTable);
    pageTable[endPageIndex] = nullptr;
}

void BlockAllocator::AllocateNewPage()
{
    PageHeader *newPage = (PageHeader *)mMemoryAllocator->AllocateBlockPage(mAllocatedPageSize, this);

    mPageTable.RecordPageInPageTable(newPage);

    PushNewPageAsHead(newPage);

    InitializePageBlocks(newPage);
}

void BlockAllocator::InitializePageBlocks(PageHeader *page)
{
    page->mFreeBlocks = (uint64_t)-1;
}

void BlockAllocator::UnlinkPage(PageHeader *pageToUnlink)
{
    if (pageToUnlink == nullptr)
    {
        return;
    }
    bool unlinked = false;
    if (mPageHead == pageToUnlink)
    {
        mPageHead = pageToUnlink->mNextPage;
        if (mPageHead != nullptr)
        {
            mPageHead->mPrevPage = nullptr;
        }
        unlinked = true;
    }

    if (mPageTail == pageToUnlink)
    {
        mPageTail = pageToUnlink->mPrevPage;
        if (mPageTail != nullptr)
        {
            mPageTail->mNextPage = nullptr;
        }
        unlinked = true;
    }

    if (!unlinked)
    {
        if (pageToUnlink->mPrevPage == nullptr)
        {
            throw "BlockAllocator::UnlinkPage failed due to a null prevPage when pageToUnlink was not mPageHead!";
        }
        else
        {
            assert(pageToUnlink->mPrevPage != pageToUnlink);
            assert(pageToUnlink->mNextPage != pageToUnlink->mPrevPage);
            pageToUnlink->mPrevPage->mNextPage = pageToUnlink->mNextPage;
            if (pageToUnlink->mNextPage != nullptr)
            {
                pageToUnlink->mNextPage->mPrevPage = pageToUnlink->mPrevPage;
            }
        }
    }
}

void BlockAllocator::AppendPage(PageHeader *ph)
{
    if (mPageTail != nullptr)
    {
        assert(mPageTail->mNextPage == nullptr);
        mPageTail->mNextPage = ph;
        ph->mNextPage = nullptr;
        ph->mPrevPage = mPageTail;
        mPageTail = ph;
    }
    else
    {
        PushNewPageAsHead(ph);
    }
}

void *BlockAllocator::Alloc()
{
    if (mPageHead == nullptr || mPageHead->mFreeBlocks == 0)
    {
        AllocateNewPage();
    }

    int bi = __builtin_ffsl(mPageHead->mFreeBlocks) - 1;
    uint64_t mask = 1LL << bi;
    mPageHead->mFreeBlocks &= ~mask;

    void *allocatedAddress = mPageHead->blockHead() + (mBlockSize * bi);

    if (mPageHead->mFreeBlocks == 0)
    {
        PageHeader *emptyPage = mPageHead;
        UnlinkPage(emptyPage);
        AppendPage(emptyPage);
    }

    return allocatedAddress;
}

void BlockAllocator::Free(void *p)
{
    PageHeader *ph = nullptr;
    size_t bi = GetBlockIndexForAddress(p, ph);
    if (bi == -1)
    {
        throw "BlockAllocator::Free received invalid address!";
    }
    uint64_t mask = 1LL << bi;
    if ((ph->mFreeBlocks & mask) != 0)
    {
        throw "BlockAllocator::Free received an unallocated address!";
    }
    ph->mFreeBlocks |= mask;

    if (ph->mFreeBlocks == ~0)
    {
        UnlinkPage(ph);
        mPageTable.RemovePageFromPageTable(ph);
        mMemoryAllocator->FreeBlockPage(reinterpret_cast<void *>(ph), mAllocatedPageSize, this);
    }
}

size_t BlockAllocator::GetBlockIndexForAddress(void *addr, PageHeader *&outPageHeader)
{
    byte *p = (byte *)addr;
    size_t bi = -1;
    //const size_t pageIndex = ComputeBlockPageIndexForAddress(addr);
    PageHeader *ph = mPageTable.GetPageForAddress(addr);
    if (ph != nullptr)
    {
        byte *pageBlocks = (byte *)ph->blockHead();
        ptrdiff_t pDistance = p - pageBlocks;
        bi = pDistance / mBlockSize;
        outPageHeader = ph;
    }
    return bi;
}

size_t PageTable::ComputeBlockPageIndexForAddress(void *addr)
{
    ptrdiff_t delta = reinterpret_cast<byte *>(addr) - mMemoryAllocatorPoolStartAddress;
    size_t index = delta / mNominalPageSize;
    return index;
}

} // namespace memory
} // namespace memtek