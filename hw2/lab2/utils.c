#include <ctype.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <stdarg.h>
#include "utils.h"
#include <dirent.h>
#define SIZE_MAX 1000

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
  /*
  Add a node to the doubly linked list
  */
  struct Node *cur = malloc(sizeof(struct Node));
  struct Node* next = head -> next;
  head -> next = cur;
  cur -> prev = head;
  cur -> next = next;
  cur -> pid = pid;
  cur -> cmd = cmd;
  next -> prev = cur;
}

struct Node *removeNode(struct Node *head, struct Node* tail, int idx){
  /*
    Remove a node to the doubly linked list
  */
  if (idx < 0)
    return NULL;
  struct Node *cur = tail -> prev;
  
  while (idx > 0 && cur != head){
    cur = cur -> prev;
    idx--;
  }
  if (cur == head)
    return NULL;
 
  struct Node* prev = cur -> prev;
  struct Node* next = cur -> next;
  prev -> next = next;
  next -> prev = prev;
  return cur;
}

void printJobs(struct Node *head, struct Node *tail){
  /*
    Print all stopped jobs from the doubly linked list
  */
  int i=1;
  tail = tail -> prev;
  while (tail && tail != head){
    printf("[%d] %s", i++, tail -> cmd);
    fflush(stdout);
    tail = tail -> prev;
  }
}

void clearList(struct Node *head){
  /*
    Free memory of the doubly linked list
  */
  while (head){
    struct Node *next = head -> next;
    free(head);
    head = next;
  }
}

bool empty(struct Node *head, struct Node *tail){
  /*
    check if double linked list is empty or not.
  */
  head = head -> next;
  if (head && head != tail)
    return false;
  return true;
}


char *readLine(){
  char *lineCmd = (char *)malloc((SIZE_MAX) * sizeof(char));
  size_t size = SIZE_MAX;
  if ((getline(&lineCmd, &size, stdin) == -1))
    return NULL;
  
  return lineCmd;
}

void nextRound(){}