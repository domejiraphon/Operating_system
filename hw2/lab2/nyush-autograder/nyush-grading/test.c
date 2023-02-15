#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>
#include <fcntl.h>

#define MAX_ARGS 100
#define MAX_LENGTH 100

int main(void) {
    while (1) {
        printf("$ ");
        char line[MAX_LENGTH];
        if (!fgets(line, MAX_LENGTH, stdin)) {
            break;
        }
        int length = strlen(line);
        if (line[length - 1] == '\n') {
            line[length - 1] = '\0';
        }
        char *args[MAX_ARGS];
        int argc = 0;
        char *token = strtok(line, " ");
        int input_redirect = 0;
        int output_redirect = 0;
        char input_file[MAX_LENGTH];
        char output_file[MAX_LENGTH];
        while (token != NULL) {
            if (strcmp(token, "<") == 0) {
                input_redirect = 1;
                token = strtok(NULL, " ");
                strcpy(input_file, token);
            } else if (strcmp(token, ">") == 0) {
                output_redirect = 1;
                token = strtok(NULL, " ");
                strcpy(output_file, token);
                
            } else {
                args[argc++] = token;
            }
            token = strtok(NULL, " ");
            
        }
        args[argc] = NULL;
       
        if (argc == 0) {
            continue;
        }
        if (strcmp(args[0], "exit") == 0) {
            break;
        }
        pid_t pid = fork();
        if (pid == 0) {
            if (input_redirect) {
                int fd = open(input_file, O_RDONLY);
                dup2(fd, 0);
                close(fd);
            }
            if (output_redirect) {
                int fd = open(output_file, O_WRONLY | O_CREAT | O_TRUNC, 0644);
                dup2(fd, 1);
                close(fd);
            }
            execvp(args[0], args);
            fprintf(stderr, "Error: command not found\n");
            exit(1);
        } else if (pid > 0) {
            int status;
            wait(&status);
        } else {
            fprintf(stderr, "Error: failed to create new process\n");
        }
    }
    return 0;
}
