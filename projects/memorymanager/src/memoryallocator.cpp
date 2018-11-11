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

using byte = u_char;


const unsigned long BYTE     = 1;
const unsigned long KILOBYTE = 1024;
const unsigned long MEGABYTE = 1024 * 1024;
const unsigned long GIGABYTE = 1024 * 1024 * 1024;
const unsigned long LARGE_PRIME = 104729;

const std::size_t MEMORY_POOL_ALIGNMENT = 64;
const std::size_t BLOCK_PAGE_ALIGNMENT = 64;
const unsigned long CACHE_LINE_ALIGNMENT_SHIFT_BUFFER = 63;


struct BlockHeader
{
    BlockHeader* mNextBlock;
};

struct PageHeader {
    uint64_t mFreeBlocks;
    PageHeader* mNextPage;
    signed char mCacheLineAlignmentOffset;
    BlockHeader* blockHead() {
        return reinterpret_cast<BlockHeader*>(this + 1);
    }
};

struct PageListHeader
{
    PageListHeader *mNextPage;
    std::size_t mPageSize;
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
    void AllocateNewPage();
    void InitializePageBlocks(PageHeader* page);
    int GetBlockIndexForAddress(void* addr, PageHeader*& outPageHeader);

    MemoryAllocator* mMemoryAllocator;
    PageHeader* mPageHead;
    size_t mBlockSize;
    size_t mPageSize;
    size_t mAllocatedPageSize;
    size_t mBlocksPerPage;
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

void * operator new(size_t size)
{
    void * p = __a.Alloc(size);
    std::cout << "new created memory at " << p << std::endl;
    return p;
}

void operator delete(void * p) noexcept
{
    std::cout << "Delete operator freed memory at " << p << std::endl;
    __a.Free(p);
}

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
    mAllocatedPool = reinterpret_cast<byte*>(malloc(mTotalMemoryBudget + MEMORY_POOL_ALIGNMENT));
    const size_t shift = MEMORY_POOL_ALIGNMENT - ((size_t)mTotalMemoryBudget % MEMORY_POOL_ALIGNMENT);
    mMemoryPool = mAllocatedPool + shift;
    mFreeMemoryList = (PageListHeader*)mMemoryPool;
    mFreeMemoryList->mNextPage = nullptr;
    mFreeMemoryList->mPageSize = mTotalMemoryBudget;
}

void MemoryAllocator::InitializeBlockAllocators() {
    const int numBlockSizes = sizeof(kBlockSizes) / sizeof(kBlockSizes[0]);
    const int maxBlockSize = kBlockSizes[numBlockSizes - 1];

    mBlockAllocators = reinterpret_cast<BlockAllocator *>(malloc(numBlockSizes * sizeof(BlockAllocator)));
    for (int i = 0; i < numBlockSizes; ++i)
    {
        //mBlockAllocators[i] = BlockAllocator(kBlockSizes[i]);
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
        BlockAllocationMap *am = mAllocationRecords + i;
        new (mAllocationRecords + i) BlockAllocationMap;
    }

    mMaxBlockSize = maxBlockSize;
}

//O(N) search. todo: consider upgrading to a tree based structure if possible
PageListHeader* MemoryAllocator::FindMemoryBlockPageForSize(size_t size, PageListHeader** outPrevPage) {
    PageListHeader* prev = nullptr;
    PageListHeader* ph = mFreeMemoryList;
    while(ph != nullptr) {
        if(ph->mPageSize >= size) {
            if(outPrevPage != nullptr) {
                *outPrevPage = prev;
            }
            return ph;
        }
        prev = ph;
        ph = ph->mNextPage;
    }
    return nullptr;
}

void* MemoryAllocator::AllocateBlockPage(std::size_t requestedSize) {
    std::size_t actualAllocSize = requestedSize + (requestedSize % 64);
    PageListHeader* prevPage = nullptr;
    PageListHeader* pageToAlloc = FindMemoryBlockPageForSize(requestedSize, &prevPage);
    PageListHeader* newFreeBlock = new (reinterpret_cast<byte*>(pageToAlloc) + actualAllocSize) PageListHeader;
    if(prevPage != nullptr) {
        prevPage->mNextPage = newFreeBlock;
    }
    newFreeBlock->mNextPage = pageToAlloc->mNextPage;
    newFreeBlock->mPageSize = pageToAlloc->mPageSize - actualAllocSize;
    
    if (pageToAlloc == mFreeMemoryList)
    {
        mFreeMemoryList = newFreeBlock;
    }
    return pageToAlloc;
}

bool MemoryAllocator::AddressIsInMemoryPool(void *p) const
{
    return p > mMemoryPool && (p < (mMemoryPool + mTotalMemoryBudget));
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
        ph = ph->mNextPage;
    }
    return nullptr;
}

void MemoryAllocator::FreeBlockPage(void *p, std::size_t requestedSize)
{
    if(!AddressIsInMemoryPool(p)) {
        throw std::invalid_argument("MemoryAllocator::FreeBlockPage received address out of pool range!");
    }

    if(!AddressIsBlockPageAligned(p)) {
        throw std::invalid_argument("MemoryAllocator::FreeBlockPage received misaligned address!");
    }

    const std::size_t actualAllocSize = requestedSize + (requestedSize % 64);
    
    PageListHeader* newReturnedPage = new (p)PageListHeader;
    if(newReturnedPage < mFreeMemoryList) {
        newReturnedPage->mNextPage = mFreeMemoryList;
        mFreeMemoryList = newReturnedPage;
    } else {
        PageListHeader *prev = FindPrevMemoryBlockPageLocationForAddress(p);
        newReturnedPage->mNextPage = prev->mNextPage; 
        prev->mNextPage = newReturnedPage;
    }
    newReturnedPage->mPageSize = actualAllocSize;
}

MemoryAllocator::MemoryAllocator()
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
        return malloc(size);
    }

    const size_t blockAllocatorIndex = mBlockSizeLookupTable[size];
    BlockAllocator* ba = mBlockAllocators + blockAllocatorIndex;
    void* block = ba->Alloc();

    const size_t blockPtrAsValue = reinterpret_cast<std::size_t>(block);

    mAllocatedPtrBackTraceMap[blockPtrAsValue] = BackTrace::CreateBackTrace();

    const size_t hashedBlockPtrValue = blockPtrAsValue % ALLOC_TABLE_HASH_SIZE;
    BlockAllocationMap& thisMap = mAllocationRecords[hashedBlockPtrValue];
    thisMap[blockPtrAsValue]=ba;

    return block;
}

void MemoryAllocator::Free(void* p) {
    try {
        const size_t blockPtrAsValue = reinterpret_cast<std::size_t>(p);
        const size_t hashedBlockPtrValue = blockPtrAsValue % ALLOC_TABLE_HASH_SIZE;

        BlockAllocationMap& allocRecordMap = mAllocationRecords[hashedBlockPtrValue];
        auto mapIter = allocRecordMap.find(blockPtrAsValue);
        if(mapIter != allocRecordMap.end()) {
            mapIter->second->Free(p);
            allocRecordMap.erase(mapIter);
        }
    } catch(const std::invalid_argument& e) {
        std::cout << "MemoryAllocator::Free error for address: " << p << std::endl;
        PrintAddressAllocCallStack(p);
        throw e;
    }
}

void MemoryAllocator::PrintAddressAllocCallStack(void* p) {
    std::cout << "ADDRESS " << p << " ALLOCATED AT: \n";
    auto mapIter = mAllocatedPtrBackTraceMap.find(reinterpret_cast<std::size_t>(p));
    if(mapIter != mAllocatedPtrBackTraceMap.end()) {
        mapIter->second.Print();
    } else {
        std::cout << "NOT FOUND." << std::endl;
    }
}

BlockAllocator::BlockAllocator( MemoryAllocator* memAllocator, size_t block_size, size_t page_maxSize)
    : mMemoryAllocator(memAllocator)
    , mPageHead(nullptr)
    , mBlockSize(block_size)
    , mPageSize(std::min(64 * block_size, page_maxSize))
    , mAllocatedPageSize(mPageSize + sizeof(PageHeader) + CACHE_LINE_ALIGNMENT_SHIFT_BUFFER)
    , mBlocksPerPage(mPageSize / mBlockSize)
{
    AllocateNewPage();
}

BlockAllocator::~BlockAllocator()
{
    //cleanup
}

void BlockAllocator::AllocateNewPage()
{
    PageHeader* newPage = (PageHeader*)mMemoryAllocator->AllocateBlockPage(mAllocatedPageSize);
    newPage->mNextPage = mPageHead;
    mPageHead = newPage;
    InitializePageBlocks(newPage);
}

void BlockAllocator::InitializePageBlocks(PageHeader* page) {
    BlockHeader* block = page->blockHead();
    /*for(int i = 0; i < mBlocksPerPage; ++i) {
        block->mNextBlock = new (reinterpret_cast<byte*>(block) + mBlockSize)BlockHeader;
        block = block->mNextBlock;
    }
    block->mNextBlock = nullptr;*/
    page->mFreeBlocks = (uint64_t)-1;
}


void* BlockAllocator::Alloc() {
    if(mPageHead->mFreeBlocks == 0) {
        AllocateNewPage();
    }

    int bi = __builtin_ffs(mPageHead->mFreeBlocks) - 1;
    uint64_t mask = 1 << bi;
    mPageHead->mFreeBlocks &= ~mask;

    return (void*)(reinterpret_cast<byte*>(mPageHead->blockHead()) + (mBlockSize * bi));
}

void BlockAllocator::Free(void* p) {
    BlockHeader* b = reinterpret_cast<BlockHeader*>(p);
    PageHeader* ph = nullptr;
    int bi = GetBlockIndexForAddress(p, ph);
    if(bi == -1) {
        throw std::invalid_argument("BlockAllocator::Free received invalid address!");
    }
    uint64_t mask = 1 << bi;
    if((ph->mFreeBlocks & mask) != 0) {
        throw std::invalid_argument("BlockAllocator::Free received an unallocated address!");
    }
    ph->mFreeBlocks |= mask;

    if(ph->mFreeBlocks == ~0) {
        mMemoryAllocator->FreeBlockPage(reinterpret_cast<void*>(ph), mAllocatedPageSize);
    }
}

int BlockAllocator::GetBlockIndexForAddress(void* addr, PageHeader*& outPageHeader) {
    byte* p = (byte*)addr;
    int bi = -1;
    PageHeader* ph = mPageHead;
    while(ph != nullptr) {
        byte* pageBlocks = (byte*)ph->blockHead();
        ptrdiff_t pDistance = p - pageBlocks;
        if(pDistance < 0 || pDistance > mPageSize) {
            continue;
        }
        if((pDistance % mBlockSize) != 0) {
            continue;
        }
        bi = pDistance / mBlockSize;
        outPageHeader = ph;
        break;
    }
    return bi;
}


