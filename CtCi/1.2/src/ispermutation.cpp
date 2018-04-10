
#include <string.h>
#include <iostream>
#include "ispermutation.h"

bool isPermutation(const char* lhs, const char* rhs) {
    
    if(!lhs || !rhs) { 
        return false;
    }

    int counts[255] = { 0 };

    const int lhsLength = strlen(lhs);
    const int rhsLength = strlen(rhs);

    if(lhsLength != rhsLength) {
        return false;
    }

    for(int i = 0; i < lhsLength; ++i) {
        counts[lhs[i]]++;
        counts[rhs[i]]--;
    }

    for(int i = 0; i < 255; ++i) {
        if(counts[i] != 0) {
            return false;
        }
    }

    return true;
}

