#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

void sigint_handler(int signum) {
  printf("Child process: Caught SIGINT, signum=%d\n", signum);
}

int main() {
  pid_t child_pid;

  child_pid = fork();
  if (child_pid == -1) {
    perror("Error forking process");
    return 1;
  } else if (child_pid == 0) {
    // This is the child process.

    // Set up signal handler.
    if (signal(SIGINT, sigint_handler) == SIG_ERR) {
      perror("Error setting up signal handler");
      return 1;
    }

    // Create a new process group.
    if (setsid() == -1) {
      perror("Error creating new process group");
      return 1;
    }

    while (1) {
      sleep(1);
      printf("Child process: Running...\n");
    }

    return 0;
  } else {
    // This is the parent process.

    sleep(3);

    // Send SIGINT to the child process.
    if (kill(child_pid, SIGINT) == -1) {
      perror("Error sending signal");
      return 1;
    }

    wait(NULL);

    return 0;
  }
}
