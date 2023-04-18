# Overview
Data compression is the process of encoding information using fewer bits than the original representation. Run-length encoding (RLE) is a simple yet effective compression algorithm: repeated data are stored as a single data and the count. In this lab, you will build a parallel run-length encoder called Not Your Usual ENCoder, or nyuenc for short.

## Usage
```
Usage: ./nyuenc [-j number of threads] filename           
```
For example:
```
make
./nyuenc -j 3 file.txt > /dev/null
```

We use two Valgrind tools, namely Helgrind and DRD, to detect thread errors in my code.
```
valgrind --tool=helgrind --read-var-info=yes ./nyuenc -j 3 file.txt > /dev/null
valgrind --tool=drd --read-var-info=yes ./nyuenc -j 3 file.txt > /dev/null
```