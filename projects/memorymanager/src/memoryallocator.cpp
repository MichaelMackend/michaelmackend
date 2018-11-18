#include <iostream>
#include "backtrace.h"
#include "mallocator11.h"
#include <utility>
#include <map>
#include <stdexcept>
#include <algorithm>
#include <cstddef>
#include <cstdlib>
#include "memoryallocator.h"
#include <stdio.h>

#define OVERLOAD_GLOBAL_NEW_DELETE

using byte = u_char;


const unsigned long BYTE     = 1;
const unsigned long KILOBYTE = 1024;
const unsigned long MEGABYTE = 1024 * 1024;
const unsigned long GIGABYTE = 1024 * 1024 * 1024;
const unsigned long LARGE_PRIME = 104729;

const std::size_t MEMORY_POOL_ALIGNMENT = 64;
const std::size_t BLOCK_PAGE_ALIGNMENT = 64;

struct BlockHeader
{
    BlockHeader* mNextBlock;
};

struct PageHeader {
    PageHeader()
    : mFreeBlocks(0)
    , mPrevPage(nullptr)
    , mNextPage(nullptr)
    {}
    uint64_t mFreeBlocks;
    PageHeader* mNextPage;
    PageHeader* mPrevPage;
    signed char mCacheLineAlignmentOffset;
    BlockHeader* blockHead() {
        return reinterpret_cast<BlockHeader*>(this + 1);
    }
};

class PageListHeader
{
    private:
    PageListHeader *mNextPage;
    public:
    PageListHeader()
     : mNextPage(nullptr)
     , mPageSize(0)
     {}

    void SetNextPage(PageListHeader* ph) {
        assert(ph != this);
        this->mNextPage = ph;
    }
    PageListHeader* NextPage() { 
        return mNextPage;
    }
    std::size_t mPageSize;
};

class PageTable
{
public:
    PageTable()
    : mPageTable0(nullptr)
    , mPageTable1(nullptr)
    , mNumPageSlots(0)
    , mMemoryAllocatorPoolStartAddress(nullptr)
    , mAllocatedPageSize(0)
    {}
    
    void InitializePageTable(size_t numPageSlots, size_t allocatedPageSize, byte* memoryAllocatorPoolStartAddress) {
        mAllocatedPageSize = allocatedPageSize;
        mMemoryAllocatorPoolStartAddress = memoryAllocatorPoolStartAddress;
        const size_t pageTableSize = sizeof(PageHeader*) * numPageSlots;
        mPageTable0 = (PageHeader**)malloc(pageTableSize);
        mPageTable1 = (PageHeader**)malloc(pageTableSize);
        for(int i = 0;i < numPageSlots; ++i) {
            mPageTable0[i] = nullptr;
            mPageTable1[i] = nullptr;
        }
    }
    size_t ComputeBlockPageIndexForAddress(void* addr);
    void RecordPageInPageTable(PageHeader* page);
    void RemovePageFromPageTable(PageHeader* page);
    PageHeader* GetPageForAddress(void* addr) {
        size_t index = ComputeBlockPageIndexForAddress(addr);
        PageHeader** pageTable = mPageTable0[index] && PageOwnsAddr(mPageTable0[index], addr)
                                 ? mPageTable0
                                 : mPageTable1[index] && PageOwnsAddr(mPageTable1[index], addr)
                                 ? mPageTable1
                                 : nullptr;
        if(pageTable == nullptr) {
            return nullptr;
        }
        return pageTable[index];
    }
    bool PageOwnsAddr(PageHeader* ph, void* addr) {
        return addr >= ph->blockHead() && addr < ((byte*)ph + mAllocatedPageSize);
    }
    
private:
    PageHeader** mPageTable0;
    PageHeader** mPageTable1;
    size_t mNumPageSlots;
    size_t mAllocatedPageSize;
    byte* mMemoryAllocatorPoolStartAddress;
};

/*
[[ph][[bh]b][[bh]b][[bh]b][[bh]b][[bh]b][[bh]b][[bh]b]]
*/
class BlockAllocator
{
public:
    BlockAllocator(
        MemoryAllocator* memAllocator,
        size_t block_size = 64,
        size_t page_maxsize = MEGABYTE);

    ~BlockAllocator();

    void* Alloc();
    void Free(void* p);

private:
    void PushNewPageAsHead(PageHeader* page);
    
    void AllocateNewPage();
    void InitializePageBlocks(PageHeader* page);
    void UnlinkPage(PageHeader* pageToUnlink);
    int GetBlockIndexForAddress(void* addr, PageHeader*& outPageHeader);

    MemoryAllocator* mMemoryAllocator;
    PageHeader* mPageHead;
    byte*  mMemoryAllocatorPoolStartAddress;
    PageTable    mPageTable;
    const size_t mBlockSize;
    const size_t mPageSize;
    const size_t mAllocatedPageSize;
    const size_t mBlocksPerPage;
};

struct BlockAllocationRecord {
    BlockAllocationRecord(void* addr, BlockAllocator* ba) : mAddr(addr), mBlockAllocator(ba) {}
    void* mAddr;
    BlockAllocator* mBlockAllocator;
};

// sorted from smallest to largest
static const size_t kBlockSizes[] =
{
  // 4-increments
  4,  8, 12, 16, 20, 24, 28, 32, 36, 40, 44, 48,
  52, 56, 60, 64, 68, 72, 76, 80, 84, 88, 92, 96,

  // 32-increments
  128, 160, 192, 224, 256, 288, 320, 352, 384,
  416, 448, 480, 512, 544, 576, 608, 640,

  // 64-increments
  704, 768, 832, 896, 960, 1024
};

static MemoryAllocator __a;

#ifdef OVERLOAD_GLOBAL_NEW_DELETE
void * operator new(size_t size)
{
    void * p = __a.Alloc(size);
    //__a.PrintAllocationSummaryReport();
    return p;
}

void operator delete(void * p) noexcept
{
    __a.Free(p);
    //__a.PrintAllocationSummaryReport();
}
#endif

void MemoryAllocator::Initialize(std::size_t total_memory) {
    __a = MemoryAllocator();
    __a.InitializeWithMemoryBudget(total_memory);
}

void MemoryAllocator::InitializeWithMemoryBudget(std::size_t memory_budget) {

    InitializeMemoryPool(memory_budget);
    InitializeBlockAllocators();
}

void MemoryAllocator::InitializeMemoryPool(std::size_t memory_budget) {
    mTotalMemoryBudget = memory_budget;
    mRemainingMemory = mTotalMemoryBudget;

    mAllocatedPool = reinterpret_cast<byte*>(malloc(mTotalMemoryBudget + MEMORY_POOL_ALIGNMENT));
    mMemoryPool = GetBlockPageAlignedAddress(mAllocatedPool);
    mFreeMemoryList = (PageListHeader*)mMemoryPool;
    
    mFreeMemoryList->SetNextPage(nullptr);
    mFreeMemoryList->mPageSize = mTotalMemoryBudget;
}

void MemoryAllocator::InitializeBlockAllocators() {
    const int numBlockSizes = sizeof(kBlockSizes) / sizeof(kBlockSizes[0]);
    const int maxBlockSize = kBlockSizes[numBlockSizes - 1];

    mBlockAllocators = reinterpret_cast<BlockAllocator *>(malloc(numBlockSizes * sizeof(BlockAllocator)));
    for (int i = 0; i < numBlockSizes; ++i)
    {
        new (mBlockAllocators + i) BlockAllocator(this, kBlockSizes[i]);
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

    mAllocationRecords = (BlockAllocationMap *)malloc(sizeof(BlockAllocationMap) * ALLOC_TABLE_HASH_SIZE);
    for (size_t i = 0; i < ALLOC_TABLE_HASH_SIZE; ++i)
    {
        //BlockAllocationMap *am = mAllocationRecords + i;
        new (mAllocationRecords + i) BlockAllocationMap;
    }

    mMaxBlockSize = maxBlockSize;
}

//O(N) search. todo: consider upgrading to a tree based structure if possible
PageListHeader* MemoryAllocator::FindMemoryBlockPageForSize(size_t size, PageListHeader** outPrevPage) {
    PageListHeader* prev = nullptr;
    PageListHeader* ph = mFreeMemoryList;
    const size_t sizeNeeded = size + sizeof(PageListHeader);
    while(ph != nullptr) {
        if(ph->mPageSize >= sizeNeeded) {
            if(outPrevPage != nullptr) {
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

void* MemoryAllocator::AllocateBlockPage(std::size_t requestedSize) {

    requestedSize = GetBlockPageAlignedSize(requestedSize);

    PageListHeader *prevPage = nullptr;
    
    PageListHeader *pageToAlloc = FindMemoryBlockPageForSize(requestedSize, &prevPage);
    
    InsertNewPageListHeaderBlock(pageToAlloc, requestedSize, prevPage);

    mRemainingMemory -= requestedSize;

    return pageToAlloc;
}

//[62] 

size_t MemoryAllocator::GetBlockPageAlignedSize(size_t size)
{
    std::size_t blockAlignmentPadding = (size % BLOCK_PAGE_ALIGNMENT);
    return size + blockAlignmentPadding;
}

byte* MemoryAllocator::GetBlockPageAlignedAddress(byte* addr) {
    return addr + (reinterpret_cast<long long>(addr) % BLOCK_PAGE_ALIGNMENT);
}

//[          ] 10
//[rrrrrrr   ] 7
//[       ppp] 

bool MemoryAllocator::AllocatedPageHasEnoughSpaceForNewPageListHeaderBlock(PageListHeader *pageToAlloc, std::size_t requestedSize){
    return (pageToAlloc->mPageSize - requestedSize) >= sizeof(PageListHeader);
}

//[          ] 10
// 0123456789
//[       ppp]

void MemoryAllocator::InsertNewPageListHeaderBlock(PageListHeader *pageToAlloc, std::size_t requestedSize, PageListHeader *prevPage){
    PageListHeader *newFreeBlock = new (reinterpret_cast<byte *>(pageToAlloc) + requestedSize) PageListHeader;
    assert(AddressIsInMemoryPool(newFreeBlock));
    newFreeBlock->SetNextPage(pageToAlloc->NextPage());
    newFreeBlock->mPageSize = pageToAlloc->mPageSize - requestedSize;

    assert(prevPage != nullptr || mFreeMemoryList == pageToAlloc);
    
    if(prevPage != nullptr) {
        prevPage->SetNextPage(newFreeBlock);
    } else
    {
        mFreeMemoryList = newFreeBlock;
    }
}
void MemoryAllocator::UnlinkEntirePageListHeaderBlock(PageListHeader *pageToAlloc, PageListHeader *prevPage){

    assert(prevPage != nullptr || mFreeMemoryList == pageToAlloc);

    if(prevPage != nullptr) {
        prevPage->SetNextPage(pageToAlloc->NextPage());
    } else {
        mFreeMemoryList = pageToAlloc->NextPage();
    }

}

bool MemoryAllocator::AddressIsInMemoryPool(void *p) const
{
    return p >= mMemoryPool && (p < (mMemoryPool + mTotalMemoryBudget));
}

bool MemoryAllocator::AddressIsBlockPageAligned(void* p) const 
{
    return (reinterpret_cast<std::size_t>(p) % BLOCK_PAGE_ALIGNMENT) == 0;
}

PageListHeader* MemoryAllocator::FindPrevMemoryBlockPageLocationForAddress(void* p) {
    if(!AddressIsInMemoryPool(p)) {
        return nullptr;
    }

    PageListHeader* prev = nullptr;
    PageListHeader* ph = mFreeMemoryList;
    while(ph != nullptr) {
        if(ph > p) {
            return prev;
        }
        prev = ph;
        ph = ph->NextPage();
    }
    return nullptr;
}

bool MemoryAllocator::PageIsAdjacentToPreviousPage(PageListHeader *page, PageListHeader *prevPage) const {
    if(page == nullptr || prevPage == nullptr) {
        return false;
    }

    return (reinterpret_cast<byte*>(prevPage) + prevPage->mPageSize) == reinterpret_cast<byte*>(page);
}

bool MemoryAllocator::TryJoinPages(PageListHeader *startPage, PageListHeader *pageToAppend) const {
    if(PageIsAdjacentToPreviousPage(pageToAppend, startPage)) {
        startPage->mPageSize += pageToAppend->mPageSize;
        startPage->SetNextPage(pageToAppend->NextPage());
        return TryJoinPages(startPage, startPage->NextPage());
    }
    return false;
}

void MemoryAllocator::FreeBlockPage(void *p, std::size_t requestedSize)
{
    if(!AddressIsInMemoryPool(p)) {
        throw "MemoryAllocator::FreeBlockPage received address out of pool range!";
    }

    if(!AddressIsBlockPageAligned(p)) {
        throw "MemoryAllocator::FreeBlockPage received misaligned address!";
    }

    ///todo validate that the free address being freed is not already part of a free blockGetBlockPageAlignedSize
    requestedSize = GetBlockPageAlignedSize(requestedSize);

    PageListHeader* newReturnedPage = new (p)PageListHeader;
    newReturnedPage->mPageSize = requestedSize;

    PageListHeader* joinStart = nullptr;
    if(newReturnedPage < mFreeMemoryList) {
        newReturnedPage->SetNextPage(mFreeMemoryList);
        mFreeMemoryList = newReturnedPage;
        joinStart = newReturnedPage;
    } else {
        PageListHeader *prev = FindPrevMemoryBlockPageLocationForAddress(p);
        assert(newReturnedPage != prev->NextPage());
        newReturnedPage->SetNextPage(prev->NextPage());
        prev->SetNextPage(newReturnedPage);
        joinStart = prev;
    }

    mRemainingMemory += requestedSize;

    TryJoinPages(joinStart, joinStart->NextPage());
}

void MemoryAllocator::PrintAllocationSummaryReport(char* buf) {
    int freeMemoryTotal = 0;
    PageListHeader* ph = __a.mFreeMemoryList;
    while(ph) {
        freeMemoryTotal += ph->mPageSize;
        ph = ph->NextPage();
    }
    //std::cout << "free page memory (" << 100.0 * (double)freeMemoryTotal / (double)__a.mTotalMemoryBudget << "%): " << freeMemoryTotal << "/" << __a.mTotalMemoryBudget << "\n";
    sprintf(buf, "free page memory %d bytes, free: %d/%d (%fpct)\n", __a.mTotalMemoryBudget - freeMemoryTotal/*__a.mRemainingMemory*/, freeMemoryTotal/*__a.mRemainingMemory*/, __a.mTotalMemoryBudget, 100.0 * (double)freeMemoryTotal / (double)__a.mTotalMemoryBudget);
}

void MemoryAllocator::CheckIntegrity() {
    int freeMemoryTotal = 0;
    PageListHeader *ph = __a.mFreeMemoryList;
    while (ph)
    {
        freeMemoryTotal += ph->mPageSize;
        ph = ph->NextPage();
    }
    assert(freeMemoryTotal == __a.mRemainingMemory);
}

MemoryAllocator::MemoryAllocator() 
: mBlockAllocators(nullptr)
, mBlockSizeLookupTable(nullptr)
, mAllocationRecords(nullptr)
, mAllocatedPool(nullptr)
, mTotalMemoryBudget(0)
{ }

MemoryAllocator::~MemoryAllocator() {
    free(mBlockAllocators);
    free(mBlockSizeLookupTable);
    free(mAllocationRecords);
    free(mAllocatedPool);
}

void* MemoryAllocator::Alloc(size_t size) {
    if(size == 0) {
        return nullptr;
    }

    if(size > mMaxBlockSize) {
        std::cout << "malloc!\n" << std::flush;
        return malloc(size);
    }

    const size_t blockAllocatorIndex = mBlockSizeLookupTable[size];
    BlockAllocator* ba = mBlockAllocators + blockAllocatorIndex;
    void* block = ba->Alloc();

    const size_t blockPtrAsValue = reinterpret_cast<std::size_t>(block);

#if RECORD_CALLSTACKS
    mAllocatedPtrBackTraceMap[blockPtrAsValue] = BackTrace::CreateBackTrace();
#endif
    
    const size_t hashedBlockPtrValue = blockPtrAsValue % ALLOC_TABLE_HASH_SIZE;
    BlockAllocationMap& thisMap = mAllocationRecords[hashedBlockPtrValue];
    thisMap[blockPtrAsValue]=ba;

    return block;
}

void MemoryAllocator::Free(void* p) {
    try {
        if(mAllocationRecords == nullptr) {
            std::cout << "free: " << p << std::endl << std::flush;
            return free(p);
        }
        
        const size_t blockPtrAsValue = reinterpret_cast<std::size_t>(p);
        const size_t hashedBlockPtrValue = blockPtrAsValue % ALLOC_TABLE_HASH_SIZE;

        BlockAllocationMap& allocRecordMap = mAllocationRecords[hashedBlockPtrValue];
        auto mapIter = allocRecordMap.find(blockPtrAsValue);
        if(mapIter != allocRecordMap.end()) {
            mapIter->second->Free(p);
            allocRecordMap.erase(mapIter);
        } else {
            
            return free(p);
        }
    } catch(const char* msg) {
        std::cout << "MemoryAllocator::Free error for address: " << p << ": " << msg << std::endl;
        PrintAddressAllocCallStack(p);
        throw msg;
    }
}

void MemoryAllocator::PrintAddressAllocCallStack(void* p) {
#if RECORD_CALLSTACKS
    std::cout << "ADDRESS " << p << " ALLOCATED AT: \n";
    auto mapIter = mAllocatedPtrBackTraceMap.find(reinterpret_cast<std::size_t>(p));
    if(mapIter != mAllocatedPtrBackTraceMap.end()) {
        mapIter->second.Print();
    } else {
        std::cout << "NOT FOUND." << std::endl;
    }
#endif
}

//[[ph][64*blocksize]]

BlockAllocator::BlockAllocator( MemoryAllocator* memAllocator, size_t block_size, size_t page_maxSize)
    : mMemoryAllocator(memAllocator)
    , mPageHead(nullptr)
    , mBlockSize(block_size)
    , mPageSize(std::min(64 * block_size, page_maxSize))
    , mAllocatedPageSize(MemoryAllocator::GetBlockPageAlignedSize(mPageSize + sizeof(PageHeader)))
    , mBlocksPerPage(mPageSize / mBlockSize)
{
    mMemoryAllocatorPoolStartAddress = mMemoryAllocator->mMemoryPool;
    const size_t pageTableLength = mMemoryAllocator->mTotalMemoryBudget / mAllocatedPageSize;
    mPageTable.InitializePageTable(pageTableLength, mAllocatedPageSize, mMemoryAllocatorPoolStartAddress);
}

BlockAllocator::~BlockAllocator()
{
    //cleanup
}

void BlockAllocator::PushNewPageAsHead(PageHeader* newPage) {
    newPage->mNextPage = mPageHead;
    if(mPageHead != nullptr) {
        mPageHead->mPrevPage = newPage;
    }
    mPageHead = newPage;
}

void PageTable::RecordPageInPageTable(PageHeader* newPage) {
    ptrdiff_t delta = reinterpret_cast<byte*>(newPage) - mMemoryAllocatorPoolStartAddress;
    size_t startPageIndex = ComputeBlockPageIndexForAddress(newPage);
    size_t endPageIndex = ComputeBlockPageIndexForAddress(reinterpret_cast<byte*>(newPage) + (mAllocatedPageSize));
    
    PageHeader** pageTable = !mPageTable0[startPageIndex] ? mPageTable0 : !mPageTable1[startPageIndex] ? mPageTable1 : nullptr;
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

void PageTable::RemovePageFromPageTable(PageHeader* page) {
    size_t startPageIndex = ComputeBlockPageIndexForAddress(page);
    size_t endPageIndex = ComputeBlockPageIndexForAddress(reinterpret_cast<byte*>(page) + (mAllocatedPageSize));
    PageHeader** pageTable = mPageTable0[startPageIndex] == page ? mPageTable0 : mPageTable1[startPageIndex] == page ? mPageTable1 : nullptr;
    assert(pageTable);
    pageTable[startPageIndex] = nullptr;
    pageTable = mPageTable0[endPageIndex] == page ? mPageTable0 : mPageTable1[endPageIndex] == page ? mPageTable1 : nullptr;
    assert(pageTable);
    pageTable[endPageIndex] = nullptr;
}

void BlockAllocator::AllocateNewPage()
{
    PageHeader* newPage = (PageHeader*)mMemoryAllocator->AllocateBlockPage(mAllocatedPageSize);
    
    mPageTable.RecordPageInPageTable(newPage);
    
    PushNewPageAsHead(newPage);
    
    InitializePageBlocks(newPage);
}

void BlockAllocator::InitializePageBlocks(PageHeader* page) {
    //BlockHeader* block = page->blockHead();
    /*for(int i = 0; i < mBlocksPerPage; ++i) {
        block->mNextBlock = new (reinterpret_cast<byte*>(block) + mBlockSize)BlockHeader;
        block = block->mNextBlock;
    }
    block->mNextBlock = nullptr;*/
    page->mFreeBlocks = (uint64_t)-1;
}

void BlockAllocator::UnlinkPage(PageHeader *pageToUnlink)
{
    if(pageToUnlink == nullptr) {
        return;
    }

    if(mPageHead == pageToUnlink) {
        mPageHead = pageToUnlink->mNextPage;
        mPageHead->mPrevPage = nullptr;
    } else {
        if(pageToUnlink->mPrevPage == nullptr) {
            throw "BlockAllocator::UnlinkPage failed due to a null prevPage when pageToUnlink was not mPageHead!";
        } else {
            assert(pageToUnlink->mPrevPage != pageToUnlink);
            assert(pageToUnlink->mNextPage != pageToUnlink->mPrevPage);
            pageToUnlink->mPrevPage->mNextPage = pageToUnlink->mNextPage;
            if(pageToUnlink->mNextPage != nullptr) {
                pageToUnlink->mNextPage->mPrevPage = pageToUnlink->mPrevPage;
            }
        }
    }
}

void* BlockAllocator::Alloc() {
    if(mPageHead == nullptr || mPageHead->mFreeBlocks == 0) {
        AllocateNewPage();
    }
    
    int bi = __builtin_ffsl(mPageHead->mFreeBlocks) - 1;
    uint64_t mask = 1LL << bi;
    mPageHead->mFreeBlocks &= ~mask;

    return (void*)(reinterpret_cast<byte*>(mPageHead->blockHead()) + (mBlockSize * bi));
}

void BlockAllocator::Free(void* p) {
    BlockHeader* b = reinterpret_cast<BlockHeader*>(p);
    PageHeader* ph = nullptr;
    int bi = GetBlockIndexForAddress(p, ph);
    if(bi == -1) {
        throw "BlockAllocator::Free received invalid address!";
    }
    uint64_t mask = 1LL << bi;
    if((ph->mFreeBlocks & mask) != 0) {
        throw "BlockAllocator::Free received an unallocated address!";
    }
    ph->mFreeBlocks |= mask;

    if(ph->mFreeBlocks == ~0) {
        UnlinkPage(ph);
        mPageTable.RemovePageFromPageTable(ph);
        mMemoryAllocator->FreeBlockPage(reinterpret_cast<void*>(ph), mAllocatedPageSize);
    }
}

int BlockAllocator::GetBlockIndexForAddress(void* addr, PageHeader*& outPageHeader) {
    byte* p = (byte*)addr;
    int bi = -1;
    //const size_t pageIndex = ComputeBlockPageIndexForAddress(addr);
    PageHeader* ph = mPageTable.GetPageForAddress(addr);
    if(ph != nullptr) {
        byte* pageBlocks = (byte*)ph->blockHead();
        ptrdiff_t pDistance = p - pageBlocks;
        bi = pDistance / mBlockSize;
        outPageHeader = ph;
    }
    return bi;
}

size_t PageTable::ComputeBlockPageIndexForAddress(void* addr) {
    ptrdiff_t delta = reinterpret_cast<byte*>(addr) - mMemoryAllocatorPoolStartAddress;
    size_t index = delta / mAllocatedPageSize;
    return index;
}


// [        |        |        |        |        |        |        ]
//     0        1       2         3        4         5       6
// [       000000000   222222222      333333333 555555555
//                1
//
// 0000000001010101011
//          121212121212121212
//                   232323232323232323
//                            3434343434343434343
//                                     454545454545454545
//                                              5656565656565656565
//                                                                6
// 0:ph0
// 1:ph0
// 2:ph2
// 3:ph2,ph3
// 4:ph3
// 5:ph5
