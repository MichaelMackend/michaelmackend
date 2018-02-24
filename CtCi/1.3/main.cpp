#include <iostream>
#include "urlify.h"
#include <string.h>


void test(const char* str) {
   
   
    if(str == nullptr) {
        std::cout << "NULL -> NULL" << std::endl;
        return;    
    }

    const unsigned length = strlen(str);
    unsigned spaces = 0;
    
    if(length == 0) {
        
        std::cout << "\"\" -> \"\"" << std::endl;
        
        return;
    }
    
    std::cout << "\"" << str << "\" -> ";

    for(unsigned i = 0; i < length; ++i) {
        if(str[i] == ' ') {
            ++spaces;
        }
    }
    const int fullLength = length + (2 * spaces) + 1;
    
    char* enlargedString = new char[fullLength];

    enlargedString[fullLength - 1] = '\0';
     
    strncpy(enlargedString, str, length);

    urlify(enlargedString, length);

    std::cout << "\"" << enlargedString << "\"" << std::endl;

    delete[] enlargedString;
}
int main(void) {

    test("hello michael");
    test("");
    test(" ");
    test("a");
    test("    ");
    test("aaaa");
    test((char*)0);


}
