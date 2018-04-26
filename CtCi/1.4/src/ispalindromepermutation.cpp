#include "ispalindromepermutation.h"
#include <string.h>
#include <iostream>

bool isPalindromePermutation(const char* str) {

    if(str == nullptr) {
        return false;
    }

    size_t length = strlen(str);

    if(length == 0) {
        return false;
    }

    if(length == 1) {
        return true;
    }

    bool odds[256];
    memset(odds,false,sizeof(odds));

    for(unsigned i = 0; i < length; ++i) {
        odds[str[i]] ^= true;
    }

    bool foundOdd = false;
    for(int i = 0; i < 256; ++i) {

        if(odds[i] && foundOdd) {
            return false;
        }
        foundOdd |= odds[i];
    }

    return true;
}



