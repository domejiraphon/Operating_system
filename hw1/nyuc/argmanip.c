/*
I don't understand variadic function that is provided in the c documentation. 
So, I read it from this website https://www.geeksforgeeks.org/variadic-functions-in-c/ and realize that
we can access the next argument by calling va_arg and point the pointer to it. 
*/
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

char **manipulate_args(int argc, const char *const *argv, int (*const manip)(int)){
  char **args;
  int stringLength;
  args = (char **)malloc((argc+1) * sizeof(char *));
  for(int i = 0; i < argc; i++, argv++) {
    stringLength = strlen(*argv);
    args[i] = (char *)malloc((stringLength+1) * sizeof(char));
    strcpy(args[i], *argv);
    //printf("%s", args[i]);
    for (int j=0; j<stringLength; j++)
      args[i][j] = manip(args[i][j]);
    args[i][stringLength] = '\0';
    //printf("%s", args[i]);
    //exit(0);
  }
  args[argc] = NULL;
  return args;
}

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

void helper(char **args){
  int length = 0;
  char **tmp=args;
  while (*tmp++)
    length++;
  
  for(int i = 0; i <= length; i++)
    free(args[i]);
    
  free(args);
}



