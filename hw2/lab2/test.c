#include <unistd.h>
#include <stdio.h>

int main() {
  char *args[] = { "wc", "-l", "input.txt", NULL };
  execvp(args[0], args);
  perror("execv");
  return 1;
}