#ifndef _UTILS_H_
#define _UTILS_H_

struct queue {
  char** arr;
  int start;
  int curSize;
};

void free_copied_args(char **args, ...);
int getLengthDoublePtr(char **ptr); 

void push(struct queue Q, char *lineCmd);

void pop(struct queue Q);

#endif
