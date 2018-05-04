#include "rotate.h"
#include <vector>

int rc(int row, int column, int dim) {
    return (row * dim) + column;
}

std::vector<int> rotateSquareMatrix(const std::vector<int>& values, int N) {

    std::vector<int> rotated(values.size());

   /* N x N
    *
    * A B C
    * D E F
    * G H I
    *
    * A -> C
    * B -> F
    * C -> I
    * D -> B
    * E -> E
    * F -> H
    * G -> A
    * H -> D
    * I -> G
    *
    *  Rule: R' = C
    *  Rule: C' = (N - 1) - R
    */

    for(int r = 0; r < N; ++r) {
        for (int c = 0; c < N; ++c) {
            rotated[rc(c, (N - 1) - r, N)] = values[rc(r, c, N)];
        }
    }

    return rotated;
}
