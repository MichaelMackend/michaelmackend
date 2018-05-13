# PROBLEM

Given two linked lists of single digit positive integers, add them togethether to produce a linked list of their sums that respect decimals.

# SOLUTION

## Constraints

What order are the number represnted? Least or most significant digits first?
Order is least to most.

## Ideas and Time/Space Complexities

### 1
My first idea would be to compose two integers out of their values and then decompose the sum of those back into a single list.  The limitation would be a number of digits.

### 2
My second idea would be iterate over both lists simultaneously, add each node toegher, and carry over the extra into each subsequent node (using mathy carry rules), ultimately potentially even appending an extra digit at the end.


## Code

I use solution #2.

## Tests

Test null.

Test empty.

Test creating extra digits.

Test multiple carry over.

Test unusual characters.

