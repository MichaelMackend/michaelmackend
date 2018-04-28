#include "isoneeditaway.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>



void doInsert(int& fi, int& ti) {
    ++ti;
}

void doDelete(int& fi, int& ti) {
    ++fi;
}

void doReplace(int& fi, int& ti) {
    ++fi;
    ++ti;
}

void doNothing(int& fi, int &ti) {
    ++fi;
    ++ti;
}

void (*determineAction(int fromLen, int toLen))(int&, int&) {
    if(fromLen == toLen) {
        return doReplace;
    }
    else if(toLen > fromLen) {
        return doInsert;
    } else {
        return doDelete;
    }
}


bool isOneEditAway(const char* fromString, const char* toString) {


    //iterating strings and test until failure using the desired operation
    const int fromLen = strlen(fromString);
    const int toLen = strlen(toString);

    int lenDiff = toLen - fromLen;
    if(abs(lenDiff) > 1) {
        return false;
    }

    void (*doEdit)(int&,int&) = determineAction(fromLen, toLen);

    int fi = 0;
    int ti = 0;
    bool acted = false;

    while(fi < fromLen && ti < toLen) {
        if(fromString[fi] == toString[ti]) {
            doNothing(fi,ti);
        }
        else if(!acted) {
            acted = true;
            doEdit(fi,ti);
        } else {
            return false;
       }
    }

    return true;
}
