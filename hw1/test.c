#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, const char *const *argv) {
  char **array;
  
  int stringLength;
  array = (char**)malloc(argc * sizeof(char*));
  const char *const * argvT = argv;
  char temp[argc][10000];
  for(int i = 0; i < argc; i++) {
    //stringLength = strlen(*argv);
    stringLength = strlen(*argv);
    //char temp[stringLength];
    strcpy(temp[i], *argv);
    for (int j=0; j<stringLength; j++){
      temp[i][j] = toupper(temp[i][j]);
    }
    //printf("%s\t%d\n", temp, stringLength);
    argv++;

    array[i] = (char*)malloc(stringLength * sizeof(char));
    array[i] = temp[i];
    //printf("%s", array[i]);
  }
  for (char *const *p = array; *p; ++argvT, ++p) {
    printf("[%s] -> [%s]\n", *argvT, *p);
  }
}
