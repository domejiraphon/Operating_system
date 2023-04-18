# Overview
The shell is the main command-line interface between a user and the operating system, and it is an essential part of the daily lives of computer scientists, software engineers, system administrators, and such. It makes heavy use of many OS features. In this lab, you will build a simplified version of the Unix shell called the New Yet Usable SHell, or nyush for short.

## Usage
Here is the grammar for valid commands:
```
[command] := ""; or
          := [cd] [arg]; or
          := [exit]; or
          := [fg] [arg]; or
          := [jobs]; or
          := [cmd] '<' [filename] [recursive]; or
          := [cmd] '<' [filename] [terminate]; or
          := [cmd] [recursive]; or
          := [cmd] [terminate] < [filename]; or
          := [cmd] [terminate].

[recursive] := '|' [cmd] [recursive]; or
            := '|' [cmd] [terminate].

[terminate] := ""; or
            := '>' [filename]; or
            := '>>' [filename].

[cmd] := [cmdname] [arg]*

[cmdname] := A string without any space, tab, > (ASCII 62), < (ASCII 60), | (ASCII 124), * (ASCII 42), ! (ASCII 33), ` (ASCII 96), ' (ASCII 39), nor " (ASCII 34) characters. Besides, the cmdname is not cd, exit, fg, jobs.

[arg] := A string without any space, tab, > (ASCII 62), < (ASCII 60), | (ASCII 124), * (ASCII 42), ! (ASCII 33), ` (ASCII 96), ' (ASCII 39), nor " (ASCII 34) characters.

[filename] := A string without any space, tab, > (ASCII 62), < (ASCII 60), | (ASCII 124), * (ASCII 42), ! (ASCII 33), ` (ASCII 96), ' (ASCII 39), nor " (ASCII 34) characters.
```
## How to run:
```
make
./nyush
```
