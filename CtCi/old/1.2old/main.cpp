#include <iostream>
#include "ispermutation.h"

void test(const char* lhs, const char* rhs) {

    std::cout << lhs << " - " << rhs << ": ";
    if(isPermutation(lhs, rhs)) {
        std::cout << "permutation!" << std::endl;
    } else {
        std::cout << "no permutation!" << std::endl;
    }
}


int main(void) {

    test("hello","bello");
    test("lolol","llloo");
    test("fight","bite");
    test("","");
    test((const char*)0,"ab");
    test("ab",(const char*)0);

}
