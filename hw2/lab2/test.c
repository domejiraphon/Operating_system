#include <ctype.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#include <stdarg.h>

#include <dirent.h>
#include<signal.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/wait.h>
int main() {
   printf("Hello 2250\n");
   int fd = open("output.txt",
                 O_CREAT|O_WRONLY|O_TRUNC,
                 S_IRUSR|S_IWUSR);
  close(1);
   dup2(fd, 1);  // duplicate the file descriptor
   close(fd);    // close the unused file descriptor
   printf("Hello 2250 again\n");
}