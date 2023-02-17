#ifndef _UTILS_H_
#define _UTILS_H_

struct Node {
  char *cmd;
  struct Node* next;
  struct Node* prev;
  pid_t pid;
};

void free_copied_args(char **args, ...);
int getLengthDoublePtr(char **ptr); 

void addNode(struct Node *head, char *cmd, pid_t pid);
pid_t removeNode(struct Node* tail, int idx);
void printJobs(struct Node *head, struct Node *tail);
void clearList(struct Node *head);
bool empty(struct Node *head, struct Node *tail);

#endif
