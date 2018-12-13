#include <iostream>
#include <sstream>
#include <exception>
#include <string>
#include <vector>
#include <thread>
#include <unistd.h>

#include "spinlock.h"

memtl::spinlock lock;

void myThreadFunc(char letter) {

    for(int i = 0; i < 100; ++i) {
        std::cout << letter;
        std::flush(std::cout);
        usleep(1);
    }
}

void myLockedThreadFunc(char letter, memtl::spinlock_exchange* lock) {
    lock->lock();
    myThreadFunc(letter);
    lock->unlock();
}

int main(void) {

    std::cout << "Hello, world!" << std::endl;

    memtl::spinlock localLock;
    memtl::spinlock localLock2(localLock);

    memtl::spinlock_exchange exchangeLock;

    std::thread t1(myLockedThreadFunc, 'a', &exchangeLock);
    std::thread t2(myLockedThreadFunc, 'b', &exchangeLock);

    t1.join();
    t2.join();
    
    return 0;
}


