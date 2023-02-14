#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>

void sigint_handler(int signum) {
    // do nothing
}

void sigtstp_handler(int signum) {
    // do nothing
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
