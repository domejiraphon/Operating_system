#include <stdio.h>
#include <stdlib.h>
#include "utils.h"


void addNode(struct Node *head, char *cmd){
  struct Node *cur = malloc(sizeof(struct Node));
  struct Node* next = head -> next;
  head -> next = cur;
  cur -> prev = head;
  cur -> next = next;
  cur -> cmd = cmd;
  next -> prev = cur;
}

void removeNode(struct Node* tail, int idx){
  struct Node *cur = tail;
  while (idx > 0){
    cur = cur -> prev;
    idx--;
  }
  struct Node* prev = cur -> prev;
  struct Node* next = cur -> next;
  prev -> next = next;
  next -> prev = prev;
}

void print(struct Node *head, struct Node *tail){
  int i=1;
  tail = tail -> prev;
  while (tail && tail != head){
    printf("[%d] %s\n", i++, tail -> cmd);
    tail = tail -> prev;
  }
}
int main() {
  struct Node *head = (struct Node *)malloc(sizeof(struct Node));
  struct Node *tail = (struct Node *)malloc(sizeof(struct Node));
  head -> next = tail;
  tail -> prev = head;
  char *cmd = "2234";
  addNode(head, cmd);
  char *cmd2 = "32";
  addNode(head, cmd2);
  char *cmd3 = "3df";
  addNode(head, cmd3);
  print(head, tail);
  removeNode(tail, 2);
  printf("\n");
   print(head, tail);

}
