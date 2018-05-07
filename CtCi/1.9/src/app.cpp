#include "app.h"
#include <string>

bool isRotatedString(const std::string& first, const std::string& second)
{
    if(first.size() != second.size())
        return false;

    if(first == second)
        return false;

    std::string extended = second + second;
    return extended.find(first) != std::string::npos;
}

