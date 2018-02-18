# PROBLEM

Determine if a stirng has all unique characters.

# SOLUTION

## Constraints

N/A

## Ideas and Time/Space Complexities

### 1
Brute force approach would be to loop over each char and test if there are any other matching characters in the rest of the string.

### 2
A slightly better approach would be to sort the string and then loop once looking for duplicates.

### 3
A slightly better approach would be to loop over each character and add the characters into a tree structure with LogN lookup. 

### 4
The best approach would be to allocate a fixed size boolean array covering each possible character.  Initialize to false.  For each character, if that array slot is false, set to true.  If already true, return that the string is not comprised of only unique characters. 

## Code

See code files.

## Tests

Test null.

Test empty.

Test a simple case with repeating.

Test a simple case without repeating.  

Test unusual characters.

