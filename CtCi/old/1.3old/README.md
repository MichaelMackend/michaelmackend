# PROBLEM

Replace all spaces in a string with %20. 

## Constraints

I can assume the string is sufficiently long.  I can assume I am given the string length.
 
# SOLUTION

## Ideas and Time/Space Complexities

### 1

The first thing to be aware of is that for each space, I will be shifting the rest of the string over by 2. 
To brute force this solution, I can iterate the string and at each space, first copy all the remaining contents of the string over by 2. 
Once the shift is complete, I am safe to replace the ' ' with '%' and follow it with a '20'.

### 2 
 
a b c d
a%20b%20c%20d

Can I iterate and store the space data in a way that I can reassemble once?

Suppose I had all the space data.
1,3,5
stored as
[0],1
[1],3
[2],5

Given that each character needs to move 2 spaces for each space encountered before it, I can assume that if a character exists past the Nth space, that character should move 2xN spaces.

By working backwards, I can start at the last character, and look at the rightmost space incidence, and copy that character over 2xN spaces.  
In fact, I can conceive of the procedure uniformly from end to start:
iterating backwards:
    for each character:
        if that character is not a space:
            copy that character 2xN spaces to the right (where N is the Nth space incidence in the space table)
        if that character IS a space:
            enter '%20' into the string starting at the position of that space. This is safe because all characters right are guaranteed to have been moved. 
END ALGORITHM

A detail, then, is to determine how to store the space indices.  The worst case scenario for spaces in terms of memory is if each character is already a space.  If that's true, the space index table needs to be as long as the string itself.  This means I am safe to allocate a space index table as long as the string.  The size of each table element needs only capture the length of the input string itself.  This could be managed with a heap alloc but I'll leave that as a minor optimization, as heap alloc/dealloc may be more consequential than a stack alloc.

## Tests

Test the following inputs:
Empty string
Null string
one character
one space
all spaces
all characters
normal string


