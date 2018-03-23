
#include <limits>
#include "isunique.h"
#include <string.h>

bool isUnique(const char* str) {

    bool chars[255] = {false};

    const int length = strlen(str);
    
    for(int i = 0; i < length; ++i) {
        if(chars[str[i]] == false) {
            chars[str[i]] = true;
        }
        return false;
    }
    return true;

}
