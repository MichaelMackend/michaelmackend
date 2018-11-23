#ifndef BAR_H
#define BAR_H
#include <iostream>

struct ThingOne {
    ThingOne() {
        std::cout << "Thing one at " << this << std::endl;
    }
    int one;
};

struct ThingTwo {
    ThingTwo() {
        std::cout << "Thing two at " << this << std::endl;
    }

    int two;
};

class Bar
{
public:
    Bar();
    ThingOne* CallOne();
    ThingTwo* CallTwo();
};

#endif // BAR_H
