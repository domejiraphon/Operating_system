# Overview
Real operating systems are virtually always written in C. We will be using C throughout this course.

This warm-up lab will reinforce your prior experience in the C programming language and give you practice using certain command-line tools. The overall comfort you’ll gain (or reinforce) with the Linux programming environment will be helpful for future labs, which will be significantly more challenging than this lab.

Specifically, this lab lets you revisit some language features specific to C:

- Pointers;
- Dynamic memory management;
- Variadic functions.

## Usage
```
./nyuc word ...
```
For example, if you run:
```
[root@... 2250]# ./nyuc Hello, world
```
It should print:
```
[./nyuc] -> [./NYUC] [./nyuc]
[Hello,] -> [HELLO,] [hello,]
[world] -> [WORLD] [world]
```