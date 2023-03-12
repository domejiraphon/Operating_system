#include <ctype.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <stdarg.h>
#include "utils.h"
#include <dirent.h>

void helper(char **args);
void free_copied_args(char **fmt, ...){
  va_list args;
  va_start(args, fmt);
  while (fmt){
    helper(fmt);
    fmt = va_arg(args, char **);
  }

  va_end(args);
}
int getLengthDoublePtr(char **args){
  int length = 0;
  char **tmp=args;
  while (*tmp++)
    length++;
  return length;
}

void helper(char **args){
  char **temp = args;
  while (*args++)
    free(*args);
  free(temp);
}