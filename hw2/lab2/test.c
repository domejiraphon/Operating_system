#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

int main() {
    int fildes[2];
    const int BSIZE = 100;
    char buf[BSIZE];
    ssize_t nbytes;
    int status;

    status = pipe(fildes);
    
    switch (fork()) {
    case -1: /* Handle error */
        break;

    case 0:  /* Child - reads from pipe */
        close(fildes[1]);                       /* Write end is unused */
        nbytes = read(fildes[0], buf, BSIZE);   /* Get data from pipe */
        /* At this point, a further read would see end-of-file ... */
        close(fildes[0]);                       /* Finished with pipe */
        exit(EXIT_SUCCESS);

    default:  /* Parent - writes to pipe */
        close(fildes[0]);                       /* Read end is unused */
        write(fildes[1], "Hello world\n", 12);  /* Write data on pipe */
        close(fildes[1]);                       /* Child will see EOF */
        exit(EXIT_SUCCESS);
    }

}
