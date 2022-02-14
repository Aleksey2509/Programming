#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <errno.h>
#include <ctype.h>
#include <unistd.h>
#include <getopt.h>
#include <poll.h>


int main()
{
    int fd[2];
    pipe(fd);

    struct pollfd* fds = calloc(1, sizeof(struct pollfd));
    char toWrite = 'a';
    fds[0].fd = fd[1];
    fds[0].events = POLLOUT | POLLERR;
    long writtenBytes = 0;

    while(1)
    {
        int status = poll(fds, 1, 0);
        if (status < 0)
        {
            printf("An error with poll occured: %s", strerror(errno));
        }
        printf("status - %d, error - %d\n", fds[0].revents & POLLOUT, fds[0].revents & POLLERR);
        if (fds[0].revents & POLLOUT)
            writtenBytes += write(fd[1], &toWrite, 1);
        else
            break;
    }

    printf("written %ld bytes\n", writtenBytes);
    close(fd[0]);
    close(fd[1]);
    free(fds);
}