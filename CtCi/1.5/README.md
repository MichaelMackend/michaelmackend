# PROBLEM

CtCI 1.5: Determine if a string is more than one edit away.

# SOLUTION

## Constraints

No contraints.

## Signature

bool canFixInOneEdit(const char* fromString, const char* toString);

## Ideas and Time/Space Complexities

1. Brute force the solution by iterating over all possibilities and comparing.
    This includes:
        Do nothing
        Try deleting each character
        Try inserting eveery possible character at every position
        Try replacing every character with every possible character at every position.

2. Optimized Brute Force solution:
    As above but with the following possible optimizations:
        Terminate early if the strings are off by more than one character length.
        Only try insert or remove if the strings are off by exactly one character length
        Only try replace if the strings are already of equal length
        Only try insert if we are one short and remove if we are one too many
        When trying insert, delete or replace, iterate left to right and fail as early as possible with the following rules:
            compare L[i] == R[i] (where L and R are input strings and i is an interating index)
            As soon as L[i] != R[i], perform the required operation.  Exit false as soon as L[i] != R[i] again.

## Code

See code files.

## Tests

Test null, empty
Matrix test true and false results as possible for all of:
 one more, one less, two more, two less, same length, repeating

