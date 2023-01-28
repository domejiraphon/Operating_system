#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

char **manipulate_args(int argc, const char *const *argv, int (*const manip)(int)){
  char **args;
  int stringLength;
  args = (char**)malloc((argc+1) * sizeof(char*));
  
  for(int i = 0; i < argc; i++, argv++) {
   
    stringLength = strlen(*argv);
    args[i] = (char*)malloc((stringLength+1) * sizeof(char));
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
void free_copied_args(char **upper_args, char **lower_args){
  char **temp = upper_args;
  while (*upper_args++)
    free(*upper_args);
  free(temp);

  temp = lower_args;
  while (*lower_args++)
    free(*lower_args);
  free(temp);
}
/*
void free_copied_args(char **upper_args){
  //int length = sizeof(upper_args) / sizeof(char *);
  int length = 0;
  char **tmp=upper_args;
  while (*tmp++)
    length++;
  
  for(int i = 0; i < length; i++){
    free(upper_args[i]);
    //free(lower_args[i]);
  }
  free(upper_args);
  //free(lower_args);
  
  
}
*/



