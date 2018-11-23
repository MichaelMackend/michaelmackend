#include<iostream>
#include<stdlib.h>
#include "bar.h"
#include <list>
#include <unistd.h>
#include <thread>
#include <random>

#include "memoryallocator.h"

using namespace std;

const unsigned long KILOBYTE = 1024;
const unsigned long MEGABYTE = 1024 * KILOBYTE;
const unsigned long GIGABYTE = 1024 * MEGABYTE;

const unsigned long TENTH_OF_A_SECOND = 100000;
const unsigned long HUNDREDTH_OF_A_SECOND = 10000;

class Foo
{
  public:
    Foo(int x, int y) : mX(x), mY(y) {
        std::cout << "Foo CTOR with x:" << mX << " y:" << mY << std::endl;
    }
    int mX;
    int mY;
    void print() {
        std::cout << "foo is at " << this << " with X:" << mX << " Y: " << mY << std::endl;
    }
};

struct X {
    char a;
    int b;
    char c;
    int d;
};

struct Y {
    int b;
    int d;
    char a;
    char c;
};

void simpleTests();
void threadTests();
void allocatorThread(const bool* quit, int* opCount);
void timerThread(const bool* quit, int* ops);
void arrayTests();

int main(int argc, char *argv[])
{
    //simpleTests();
    threadTests();
    //arrayTests();
}

void arrayTests() {
    cout << "start array tests...\n";
    MemoryAllocator::Initialize(100 * KILOBYTE);

    cout << "allocate arrays...\n";
    int* array1 = new int[10];
    int* array2 = new int[20];
    int* array3 = new int[30];
    
    cout << "deallocate arrays...\n";
    delete[] array1;
    delete[] array2;
    delete[] array3;

    cout << "done";
    return;
}

#define NUM_THREADS 4

void threadTests() {
    
    bool quit = false;
    cout << "running...";
    MemoryAllocator::Initialize(100 * MEGABYTE);
    int ops[NUM_THREADS] = {0};
    std::list<std::thread> threads;
    for(int i = 0; i < NUM_THREADS; ++i) {
        threads.push_back(std::thread(allocatorThread, &quit, &(ops[i])));
    }

    threads.push_back(std::thread(timerThread, &quit, ops));
    
    //std::thread t(allocatorThread, &quit, );
    cin.get();
    quit = true;
    //t.join();
    auto thread_iter = threads.begin();
    while(thread_iter != threads.end()) {
        thread_iter->join();
        thread_iter++;
    }
    cout << "done\n";
    return;
}

#define ALLOC_SLOTS 800
#define MIN_ALLOC 512
#define MAX_ALLOC 1024

void printMemSummary(char* logBuf, long long allocated) {
    MemoryAllocator::PrintAllocationSummaryReport(logBuf);
    cout << "for " << allocated << " bytes: " << logBuf;
    flush(cout);
}

template <typename unit, typename startType>
void printTimeSummary(startType start, startType finish, double iter)
{
    //using unit = std::chrono::milliseconds;
    //auto finish = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<unit>(finish - start).count();
    std::cout << std::fixed << "perf: " << duration / iter << std::endl;
    flush(cout);
}

void timerThread(const bool* quit, int* ops) {
    using unit = std::chrono::milliseconds;
    auto start = std::chrono::high_resolution_clock::now();
    auto lastCheck = start;
    while(!*quit) {
        long long opCount = 0;
        for(int i = 0; i < NUM_THREADS; ++i) {
            opCount += ops[i];
        }
        
        auto now = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<unit>(now - lastCheck).count();
        if ((duration >= 2000))
        {
            //std::cout << "Update with " << opCount << " ops!" << std::endl;
            lastCheck = now;
            printTimeSummary<unit>(start, now, (double)opCount);
            //printMemSummary(logBuf, allocated);
        }
    }
}

void allocatorThread(const bool *quit, int* opCount)
{
    //int ids[10000];
    //int mems[10000];
    std::random_device rd;
    std::mt19937 mt(0);
    std::uniform_int_distribution<int> slot(0, ALLOC_SLOTS - 1);
    std::uniform_int_distribution<int> mem(MIN_ALLOC, MAX_ALLOC);
    
    

    char *allocs[ALLOC_SLOTS] = {nullptr};
    long long mems[ALLOC_SLOTS] = {-1};
    int allocations = 0;
    bool done = false;
    int countdown = 4;
    long long iter = 0;
    using unit = std::chrono::milliseconds;
    auto start = std::chrono::high_resolution_clock::now();
    std::size_t allocated = 0;
    char logBuf[512] = {'\0'};
    
    while (!done)
    {
        int i = slot(mt);
        
        ++iter;
        if(allocs[i] != nullptr) {
            allocated -= mems[i];
            mems[i] = -1;
            delete[] allocs[i];
            allocs[i] = nullptr;
        } else {
            int size = mem(mt);
            allocated += size;
            allocs[i] = new char[size];
            mems[i] = size;
        }
        *opCount += 1;
        //cout << std::this_thread::get_id() << endl;
    }
}

void simpleTests() {
    try
    {
        MemoryAllocator::Initialize(GIGABYTE);

        //MemoryAllocator::PrintAllocationSummaryReport();

        int *pInts[3][64] = {nullptr};
        char logBuf[512] = {'\0'};

        for (int i = 0; i < 3; ++i)
        {
            for (int j = 0; j < 64; ++j)
            {
                pInts[i][j] = new int;
            }
        }

        MemoryAllocator::PrintAllocationSummaryReport(logBuf);
        cout << logBuf << endl;

        for (int i = 0; i < 64; ++i)
        {
            delete pInts[1][i];
        }

        MemoryAllocator::PrintAllocationSummaryReport(logBuf);
        cout << logBuf << endl;

        for (int i = 0; i < 64; ++i)
        {
            delete pInts[2][i];
        }

        MemoryAllocator::PrintAllocationSummaryReport(logBuf);
        cout << logBuf << endl;
        
        for (int i = 0; i < 64; ++i)
        {
            delete pInts[0][i];
        }

        MemoryAllocator::PrintAllocationSummaryReport(logBuf);
        cout << logBuf << endl;
        
        X *x = new X;
        Y *y = new Y;

        std::cout << "SIZEOF X: " << sizeof(*x) << std::endl;
        std::cout << "SIZEOF Y: " << sizeof(*y) << std::endl;

        delete x;
        delete y;
        //delete y;

        MemoryAllocator::PrintAllocationSummaryReport(logBuf);
        cout << logBuf << endl;
        
        Foo *foo = new Foo(3, 5);
        foo->print();
        foo->mX = 6;
        foo->print();

        Bar *bar = new Bar();
        ThingOne *one = bar->CallOne();
        ThingTwo *two = bar->CallTwo();

        MemoryAllocator::PrintAllocationSummaryReport(logBuf);
        cout << logBuf << endl;
        
        //std::cout << "Foo lists: " << std::endl;
        //std::list<Foo> fooList;
        //fooList.push_back(Foo(3,4));
        //fooList.push_back(Foo(5,6));

        delete foo;
        delete two;

        MemoryAllocator::PrintAllocationSummaryReport(logBuf);
        cout << logBuf << endl;
        
        delete bar;
        delete one;

        //delete one;

        MemoryAllocator::PrintAllocationSummaryReport(logBuf);
        cout << logBuf << endl;
    }
    catch (const char *msg)
    {
        std::cout << "Aborting program due to uncaught exception: " << msg << std::endl;
        return;
    }

    //MemoryAllocator::PrintAllocationSummaryReport(logBuf);
    return;
}
