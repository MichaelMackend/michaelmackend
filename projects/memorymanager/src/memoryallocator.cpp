#include <iostream>
#include "backtrace.h"
#include "mallocator11.h"
#include <utility>
#include <map>
#include <stdexcept>
#include <algorithm>
#include <cstddef>
#include "memoryallocator.h"


const unsigned long BYTE     = 1;
const unsigned long KILOBYTE = 1024;
const unsigned long MEGABYTE = 1024 * 1024;
const unsigned long GIGABYTE = 1024 * 1024 * 1024;
const unsigned long LARGE_PRIME = 104729;

struct BlockHeader {
    BlockHeader* mNextBlock;
};

struct PageHeader {
    uint64_t mFreeBlocks;
    PageHeader* mNextPage;
    BlockHeader* blockHead() {
        return reinterpret_cast<BlockHeader*>(this + 1);
    }
};

/*
[[ph][[bh]b][[bh]b][[bh]b][[bh]b][[bh]b][[bh]b][[bh]b]]
*/
class BlockAllocator
{
public:
    BlockAllocator(size_t block_size = 64,
                   size_t page_maxsize = MEGABYTE);

    ~BlockAllocator();

    void* Alloc();
    void Free(void* p);

private:
    void AllocateNewPage();
    void InitializePageBlocks(PageHeader* page);
    int GetBlockIndexForAddress(void* addr, PageHeader*& outPageHeader);

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

void MemoryAllocator::Initialize() {
    __a = MemoryAllocator();
}

MemoryAllocator::MemoryAllocator()
{
    const int numBlockSizes = sizeof(kBlockSizes) / sizeof(kBlockSizes[0]);
    const int maxBlockSize = kBlockSizes[numBlockSizes - 1];

    mBlockAllocators = reinterpret_cast<BlockAllocator*>(malloc(numBlockSizes * sizeof(BlockAllocator)));
    for(int i = 0; i < numBlockSizes; ++i) {
        //mBlockAllocators[i] = BlockAllocator(kBlockSizes[i]);
        new (mBlockAllocators + i)BlockAllocator(kBlockSizes[i]);
    }

    mBlockSizeLookupTable = reinterpret_cast<size_t*>(malloc((maxBlockSize + 1) * sizeof(size_t)));
    size_t blockSizeIndex = 0;
    for(size_t i = 0; i < maxBlockSize; ++i) {
        if(i > kBlockSizes[blockSizeIndex]) {
            blockSizeIndex++;
        }
        mBlockSizeLookupTable[i] = blockSizeIndex;
    }

    mAllocationRecords = (BlockAllocationVector*)malloc(sizeof(BlockAllocationVector) * ALLOC_TABLE_HASH_SIZE);
    for(size_t i = 0; i < ALLOC_TABLE_HASH_SIZE; ++i) {
        BlockAllocationVector* ar = mAllocationRecords + i;
        new (mAllocationRecords + i)BlockAllocationVector;
        ar->reserve(10);
    }
/*      for(auto allocRecordVector : mAllocationRecords) {
        allocRecordVector.reserve(10);
    }
    */

    mMaxBlockSize = maxBlockSize;
}

MemoryAllocator::~MemoryAllocator() {

}

void* MemoryAllocator::Alloc(size_t size) {
    if(size == 0) {
        return nullptr;
    }

    if(size > mMaxBlockSize) {
        return nullptr;
    }

    const size_t blockAllocatorIndex = mBlockSizeLookupTable[size];
    BlockAllocator* ba = mBlockAllocators + blockAllocatorIndex;
    void* block = ba->Alloc();
    const size_t blockPtrAsValue = reinterpret_cast<std::size_t>(block);
    mAllocatedPtrBackTraceMap[blockPtrAsValue] = BackTrace::CreateBackTrace();
    const size_t hashedBlockPtrValue = blockPtrAsValue % ALLOC_TABLE_HASH_SIZE;
    mAllocationRecords[hashedBlockPtrValue].push_back(BlockAllocationRecord(block, ba));
    return block;
}

void MemoryAllocator::Free(void* p) {
    try {
        const size_t blockPtrAsValue = reinterpret_cast<std::size_t>(p);
        const size_t hashedBlockPtrValue = blockPtrAsValue % ALLOC_TABLE_HASH_SIZE;
        mBlockAllocators[hashedBlockPtrValue].Free(p);
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

BlockAllocator::BlockAllocator(size_t block_size,
                               size_t page_maxSize)
    : mPageHead(nullptr)
    , mBlockSize(block_size)
    , mPageSize(std::min(64 * block_size, page_maxSize))
    , mAllocatedPageSize(mPageSize + sizeof(PageHeader))
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
    PageHeader* newPage = (PageHeader*)malloc(mAllocatedPageSize);
    newPage->mNextPage = mPageHead;
    mPageHead = newPage;
    InitializePageBlocks(newPage);
}

void BlockAllocator::InitializePageBlocks(PageHeader* page) {
    BlockHeader* block = page->blockHead();
    for(int i = 0; i < mBlocksPerPage; ++i) {
        block->mNextBlock = new (reinterpret_cast<char*>(block) + mBlockSize)BlockHeader;
        block = block->mNextBlock;
    }
    block->mNextBlock = nullptr;
    page->mFreeBlocks = (uint64_t)-1;
}


void* BlockAllocator::Alloc() {
    if(mPageHead->mFreeBlocks == 0) {
        AllocateNewPage();
    }

    int bi = __builtin_ffs(mPageHead->mFreeBlocks) - 1;
    uint64_t mask = 1 << bi;
    mPageHead->mFreeBlocks &= ~mask;

    return (void*)(reinterpret_cast<char*>(mPageHead->blockHead()) + (mBlockSize * bi));
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
}

int BlockAllocator::GetBlockIndexForAddress(void* addr, PageHeader*& outPageHeader) {
    char* p = (char*)addr;
    int bi = -1;
    PageHeader* ph = mPageHead;
    while(ph != nullptr) {
        char* pageBlocks = (char*)ph->blockHead();
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


