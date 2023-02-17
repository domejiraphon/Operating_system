#include <ctype.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <stdarg.h>
#include "utils.h"

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


void addNode(struct Node *head, char *cmd, pid_t pid){
  struct Node *cur = malloc(sizeof(struct Node));
  struct Node* next = head -> next;
  head -> next = cur;
  cur -> prev = head;
  cur -> next = next;
  cur -> pid = pid;
  cur -> cmd = cmd;
  next -> prev = cur;
}

pid_t removeNode(struct Node* tail, int idx){
  if (idx < 1)
    return -1;
  struct Node *cur = tail;
  while (idx > 0 && cur){
    cur = cur -> prev;
    idx--;
  }
  if (idx != 0)
    return -1;
  struct Node* prev = cur -> prev;
  struct Node* next = cur -> next;
  prev -> next = next;
  next -> prev = prev;
  pid_t out = cur -> pid;
  free(cur);
  return out;
}

void printJobs(struct Node *head, struct Node *tail){
  int i=1;
  tail = tail -> prev;
  while (tail && tail != head){
    printf("[%d] %s", i++, tail -> cmd);
    fflush(stdout);
    tail = tail -> prev;
  }
}

void clearList(struct Node *head){
  while (head){
    struct Node *next = head -> next;
    free(head);
    head = next;
  }
}

bool empty(struct Node *head, struct Node *tail){
  int i=0;
  head = head -> next;
  while (head && head != tail){
    head = head -> next;
    i++;
  }
  return i == 0;
}

