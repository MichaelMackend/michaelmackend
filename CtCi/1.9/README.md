# PROBLEM

Determine if a string is a rotation of another.

# SOLUTION

## Constraints

I may only call IsSubstring once.

## Signature

bool isRotation(const char* first, const char* second);

## Ideas and Time/Space Complexities

### 1
How would I brute for this?  I could do it in N^2 time.  Testing for a rotation amounts to doing a char by char comparison between the two strings but with an offset in the indices.  Brute forcing this problem would amount to executing this process from 0 offset to N - 1 offset, returning as soon it finds a match or at the end if it doesn't.  

### 2

michael
chaelmi chaelmi

Meeting the constraint in the problem definition, I could simply extend the roation of the second string by it's full length again and call IsSubstring once.  If it's a rotation it would guarantee to return true.  In reality, this is no different than the process for answer #1, but it does meet the constraint.  I will go with this as it is likely the intended solution.


## Code

See code files.

## Tests

Test null.

Test empty.

Test a simple case with repeating.

Test a simple case without repeating.  

Test unusual characters.

