#include "app.h"
#include <vector>

int rc(int row, int col, int dimension) {
    return (row * dimension) + col;
}


std::vector<int> zeroMatrix(const std::vector<int>& values, int dimension) {

    const int N = dimension;
    std::vector<int> zeroed = values;


    std::vector<bool> zeroRow(dimension, false);
    std::vector<bool> zeroCol(dimension, false);

    for(int r = 0; r < N; ++r) {
        for(int c = 0; c < N; ++c) {
            if(values[rc(r,c,N)] == 0) {
                zeroRow[r] = true;
                zeroCol[c] = true;
            }
        }
    }

    for(int r = 0; r < N; ++r) {
        for(int c = 0; c < N; ++c) {
            if(zeroRow[r] || zeroCol[c]) {
                zeroed[rc(r,c,N)] = 0;
            }
        }
    }

    return zeroed;
}

