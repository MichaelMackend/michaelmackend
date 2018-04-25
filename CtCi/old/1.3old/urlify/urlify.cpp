#include "urlify.h"
#include <string.h>

void urlify(char* str, int length) {

    if(str == nullptr) {
        return;
    }
    if(length == 0) {
        return;
    }
    unsigned* spaces = new unsigned[length] { 0U };
    unsigned spaceIndex = 0U;

    for(int i = 0; i < length; ++i) {
        if(str[i] == ' ') {
            spaces[spaceIndex++] = i;
        }
    }

    if(spaceIndex == 0) {
        delete[] spaces;
        return;
    }

    spaceIndex--;

    for(int i = length - 1; i >= 0; --i) {
        if(i == spaces[spaceIndex]) {
            strncpy(str + i + (2 * spaceIndex), "%20", 3);    
            if(spaceIndex == 0) {
                delete[] spaces;
                return;    
            }
            spaceIndex--;
        } else if(i > spaces[spaceIndex]) {
            const unsigned offset = 2 * (spaceIndex + 1);
            str[i + offset] = str[i];
        }
    }
    delete[] spaces;
}

