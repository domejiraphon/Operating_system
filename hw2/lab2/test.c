#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>

int main() {
  
  int fd = open("output.txt", O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);
  dup2(fd, 1);
  
  close(fd);

  printf("This will be written to output.txt\n");

  return 0;
}
