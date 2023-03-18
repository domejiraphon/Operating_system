#ifndef _UTILS_H_
#define _UTILS_H_
typedef struct Task {
  int a, b;
  int id;
} Task;

void free_copied_args(char **args, ...);
int getLengthDoublePtr(char **ptr); 

#endif
