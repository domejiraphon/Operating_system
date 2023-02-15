#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>

struct queue {
  char* arr[100];
  int start;
  int curSize;
};
void push(struct queue Q, char *lineCmd){
  char *str = (char *)malloc((strlen(lineCmd) + 1) * sizeof(char));
  strcpy(str, lineCmd);
  Q.arr[curSize++] = str;
 
  Q.arr[curSize] = NULL;
  printf("%d %s\n",curSize, Q.arr[0]);//exit(0);
}
void pop(struct queue Q){
  free(Q.arr[Q.start++]);
}

int main() {
    // ignore SIGINT and SIGTSTP signals in parent process
    signal(SIGINT, SIG_IGN);
    signal(SIGTSTP, SIG_IGN);

    // fork child process
    pid_t pid = fork();
    if (pid == -1) {
        perror("fork");
        exit(1);
    } else if (pid == 0) {
        // child process
        // execute a command
        execlp("sleep", "sleep", "10", NULL);
        exit(0);
    } else {
        // parent process
        // wait for child to finish or stop
        int status;
        waitpid(pid, &status, WUNTRACED);
        if (WIFSTOPPED(status)) {
            // child process has stopped
            printf("Child process stopped with signal %d\n", WSTOPSIG(status));
        } else {
            // child process has terminated
            printf("Child process finished with status %d\n", status);
        }
    }

    return 0;
}
