#include <iostream>
#include "isunique.h"

void testString(const char* str) {
    std::cout << "String: \"";
    std::cout << str << "\" is ";
    if(isUnique(str)) {
        std::cout << "unique!" << std::endl;
    } else {
        std::cout << "NOT unique!" << std::endl;
    }
}


int main(void) {

    testString("hello");
    testString("helpo");

}


