# Vectorized Base64 Encoder/Decoder for RISC-V RVV

This repository contains a implementation of the Base64 encoding and decoding algorithms, optimized for the RISC-V architecture with the RVV (RISC-V Vector Extension). The implementation utilizes both C and intrinsic assembly functions to leverage the vector processing capabilities of the RVV extension.

## About Base64

Base64 is a binary-to-text encoding scheme that represents binary data in an ASCII string format by translating it into a radix-64 representation. It is commonly used when there is a need to encode binary data, especially when that data needs to be stored and transferred over media that are designed to deal with textual data. This encoding helps to ensure that the data remains intact without modification during transport.

Base64 encoding involves dividing every three bytes of binary data into four groups of six bits each. Each group of six bits is used as an index into an array of 64 printable characters. The resulting characters are output as a string of ASCII characters. Padding characters (`=`) are added to the output to make its length a multiple of 4, which is necessary for the decoding process.

## Project Structure

- **src/**: Contains the source files written in C, utilizing intrinsic assembly functions to access the vector capabilities of the RISC-V RVV extension.
- **include/**: Includes header files with function declarations and macro definitions.

## Building the Project

To build the encoder and decoder, ensure you have a RISC-V toolchain with support for the RVV extension installed. You can compile the project using the following commands:

```bash
mkdir build
make
```
