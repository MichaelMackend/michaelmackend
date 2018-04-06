

#include "a.h"
#include <iostream>

class Test : public A<int> {

    void testMethod() {

    }
};

int main(void) {

    Test t;

    return 0;
}
