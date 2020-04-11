# SimpleCipherCryptanalysis
Academic attempt to implement Simple block cipher and linear cryptanalysis algorithm.

## Abstract
This project is an academic attempt, as a student, to implement the ***Simple*** block cipher (16 bit block cipher, with 80 bit key) and to perform linear cryptanalysis to get at least a subset of the bits of the last 16 bit round key in order to decrease the number of attempts needed to bruteforce the whole key.

## The cipher
The cipher is implemented as a SPN block cipher. It consists of 3 rounds of *Add_Round_Key*, *Substitution Layer* (SBOXES) and *Permutation Layer*. The last round is composed by *Add_Round_Key*, *Substitution Layer*, *Add_Round_Key*.

## Linear Cryptanalysis
The program tries to find a good relation between some bits of the plaintext and some bits of the last round sboxes input which should approximate, at least locally, the behaviour of the cipher with a certain theoretical probability, different from 0.5.

Then, it generates a random set of <ptx, ctx> pairs (ptx -> plaintext; ctx -> ciphertext) computed with a random key, and tries to guess some of the bits of the last round by computing the actual probability, over the whole set, that the relation holds with all the possible subkeys. 

The subkeys having the highest bias (in absolute value) are considered to be the most probable correct guesses.

Assuming to know the 64 Most Significant Bits of the key (in real applications they should have been brute forced or cryptanalysis should go on also for previous rounds), all the combinations of the subkeys (at most 64, usually) are tested in order to find the correct ast round key. This analysis, reduces the key guessing efforts from 2^80 to 2^64 * 2^6 = 2^70, thus reducing the effort of a factor of 1024.

By iterating the analysis on previous rounds, it is probably possible to further reduce the effort. This is left as a possible future work.

## Comments
For now, the analysis does not always work. Some times, it is not able to find the correct key. This may be due to the random choice of pairs <ptx, ctx>, which could give actual biases which does not reflect the expected biases. In such cases, it is usually effective to repeat the analysis with a different set. Most of the times, however (more than 80% of the times) the analysis returns the last round subkey.

***NOTE**: this project is to be considered as it is: an academic **individual** project, developed in dead times between personal studies, and thought to try and get in depth knowledge about linear cryptanalysis. This is not a guide or a manual or an actual implementation of a cipher or of the linear cryptanalysis algorithm.
Much of the complexity is simplified in the context with respect to real secure ciphers.*

## Credits
The cipher structure and images are taken from "*Block Cipher Cryptanalysis - Part 1* of professor ***Gerardo Pelosi*** from the *Department of Electronics, Information and Bioengineering* (DEIB) at *Politecnico di Milano*.
