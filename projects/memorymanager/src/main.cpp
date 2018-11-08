#include<iostream>
#include<stdlib.h>
#include "bar.h"
#include <list>
#include <unistd.h>
#include "memoryallocator.h"

using namespace std;

const unsigned long GIGABYTE = 1024 * 1024 * 1024;

class Foo {
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

int main(int argc, char *argv[])
{
    MemoryAllocator::Initialize(GIGABYTE);
    //unsigned long val = 1;
    //std::cout << "sizeof(long): " << 8 * sizeof(val) << std::endl;
    //std::cout << "MSB for 5: " << __builtin_clzl(val) << std::endl;
    //return 0;
    sleep(1);
    X* x = new X;
    Y* y = new Y;

    std::cout << "SIZEOF X: " << sizeof(*x) << std::endl;
    std::cout << "SIZEOF Y: " << sizeof(*y) << std::endl;

    delete x;
    delete y;
    delete y;

    Foo* foo = new Foo(3,5);
    foo->print();
    foo->mX = 6;
    foo->print();

    Bar* bar = new Bar();
    ThingOne* one = bar->CallOne();
    ThingTwo* two = bar->CallTwo();

    std::cout << "Foo lists: " << std::endl;
    std::list<Foo> fooList;
    fooList.push_back(Foo(3,4));
    fooList.push_back(Foo(5,6));

    delete foo;
    delete two;
    delete bar;
    delete one;
    return 0;
}
