# SimpleCipherCryptanalysis
Academic attempt to implement Simple block cipher and linear cryptanalysis algorithm.

## Abstract
This project is an academic attempt, as a student, to implement the ***Simple*** block cipher (16 bit block cipher, with 80 bit key) and to perform linear cryptanalysis to get at least a subset of the bits of the last 16 bit round key in order to decrease the number of attempts needed to bruteforce the whole key.

## The cipher
The cipher is implemented as a SPN block cipher. It consists of 3 rounds of *Add_Round_Key*, *Substitution Layer* (SBOXES) and *Permutation Layer*. The last round is composed by *Add_Round_Key*, *Substitution Layer*, *Add_Round_Key*.

## Linear Cryptanalysis
The program tries to find a good relation between some bits of the plaintext and some bits of the last round sboxes input which should approximate, at least locally, the behaviour of the cipher with a certain theoretical probability, different from 0.5.
Then, it generates a random set of <ptx, ctx> pairs (ptx -> plaintext; ctx -> ciphertext) computed with a random key, and tries to guess some of the bits of the last round by computing the actual probability, over the whole set, that the relation holds with all the possible subkeys. 
The subkey having the highest bias (in absolute value) is considered to be the correct guess.

## Comments
For now, the analysis does not always work. Many times it does the wrong guess because there are 2 different subkeys with the very same probabilty, and it keeps the first one it tries.
Some other times, the guessed subkey is terribly wrong.
Working on fixing this behaviour to try and make the guess as much as correct as possible.
