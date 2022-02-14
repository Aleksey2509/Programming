#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <ctype.h>
#include <pwd.h>
#include <grp.h>
#include <unistd.h>
#include <uuid/uuid.h>
#include <errno.h>
#include <fcntl.h>
#include <time.h>
#include <sys/stat.h>
#include <sys/param.h>
#include <sys/types.h>
#include <sys/sem.h>
#include <sys/msg.h>
#include <sys/shm.h>
#include <sys/time.h>

int main()
{
    int fd[2];
    pipe(fd);
    long writtenBytes = 0;
    char toWrite = 'a';
    

    while(1)
    {
        writtenBytes += write(fd[1], &toWrite, 1);
        printf("written - %ld\n", writtenBytes);
    }
}