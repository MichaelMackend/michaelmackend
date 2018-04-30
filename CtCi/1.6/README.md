# PROBLEM

Compress a string by representing repeated characters numerically:

aaabbcccc => a3b2c4

Only return the compressed version of the string if it is shorter.

# SOLUTION

## Constraints

N/A

## Signature

input:
a string

return:
a string

const char* compressString(const char* inputString);

## Ideas and Time/Space Complexities

### 1

Iterate forward, keeping track of the current letter and count.  When a letter changes, append the changes to some other string buffer meant for writing to.  If we reach the end of that string buffer, we can give up and return the original string.

Is there a way to know ahead of time whether the new string will be longer.  I can't think of any.  Because of this, we will absolutely need a new string buffer to be created.

## Code

See code files.

## Tests


