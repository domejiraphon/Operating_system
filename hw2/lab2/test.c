#include <stdio.h>
#include <unistd.h>
int main() {
 printf("one");
 execl("/bin/echo", "/bin/echo", "one", "two", "three", NULL);
 printf("three");
}