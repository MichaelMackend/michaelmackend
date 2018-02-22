# PROBLEM

Given 2 strings, write a function to determine whether one is a permutation of another.

# SOLUTION

## Constraints

No constriants

## Ideas and Time/Space Complexities

A permutation of a string is a string with the same incidences of characters.  ie. 

hello ehlol
bobby obbby
bob   obb

### 1

Count the characters in each string and compare the results. The containers for the counts matters.
I could count them into a Dictionary or HashTable and then iterate the tables to check for equality.
I could count them into an int array[255] and compare the arrays.  

2N for the sums (2 lgN if a tree type map is used)
1 for the comparison
O(N)

### 2 

Sort each string and them compare them.  

2 NlgN for the sorts
N for the compare
O(NlgN)

### 3

Compute numerical hashes for each string that matches only for permutations.
Compare hashes.  

I can't think of a good hash off the top of my head, but the O(N) compels me to go with idea 1.

2N for the hashing
O(N) but likely superior.

## Tests

Test normal permutation
Test normal non-permutation
Test all the same characters
Test none of the same characters
Test null 
Test empty
