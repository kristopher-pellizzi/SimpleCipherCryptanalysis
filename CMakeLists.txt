cmake_minimum_required(VERSION 3.15)
project(SimpleCipher C)

set(CMAKE_C_STANDARD 99)

add_executable(SimpleCipher src/main.c src/bitvector.c src/bitvector.h src/linear_cryptanalysis.h src/linear_cryptanalysis.c src/sbox.h src/list.h src/list.c src/couple.h)