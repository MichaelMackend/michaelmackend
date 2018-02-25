# PROBLEM

Given a string, write a function to checek if it is a permutation of a palindrome.  

What is a palindrome?
A palindrome is a word that is the same forwards as it is backwards. 

What is a permutation?
A permutation of a given string is any other string which has the same character incidence count as the given string.

What is a hypothetical signature?
bool isPalindromePermutation(const char* str);

# SOLUTION
## Constraints
Spaces are not considered when evaluating palindromes. 
## Ideas and Time/Space Complexities
### 1
The first thing I'm asking myself is whether there are any properties of palindromes relevant to determining permutations.  
If a permutation is merely a matched count of characters, is there some quality of a palindrome as far as character counts
go that we might leverage?

To investigate this possibility, I look to some examples:

Some simple palindromes:
aca
acca
aaccaa
adddda
addda
addddddda
acaddaca
acxccbbybbccxca

What I am observing is that all palindromes have a particular rule:
For any given character in a palindrome string, there must exist another such character on the other side EXCEPT in the case of the center character in an odd length string.

I will investigate the longest of my examples:
a:2
c:6
x:2
b:4
y:1

Given the requirement of pairing, I can see that we can conclude that no more than ONE character may be of an odd count.  Moreover, that odd count character must have a single incidence . 

So the function can operate as follows:
count all characters, ignoring spaces (due to the Constraint)
then, iterate over all the characters counted and return false if any character has the propery <count>%2==1 and <count> > 1
if we encounter any character <count> == 1, note it.  if we encounter a second <count>==1, return false;
if we never return false, return true.

The runtime complexity of this operation will be O(N) for the counting.  Everything else is constant time and reduces to O(1).

### 2
For the sake of exercise, I will consider a brute force solution. 
Supposing that I ignore the useful aforementioned propery, I would have to do the following:
Generate all permutations of the given string.
For each permutation, test if the string is a palindrome. 
Testing for palindrome is likely O(N), but generating all permutations is enormous.  
From combinatorial studies, we know that for a given string, there are N! permutations (taken from "N choose R" in discrete math)

## Tests

* Test NULL
* Test empty
* Test all spaces
* Test all the same characters
* Test negative case with no spaces
* Test negative case with spaces
* Test positive case with no spaces
* Test positive case with spaces

