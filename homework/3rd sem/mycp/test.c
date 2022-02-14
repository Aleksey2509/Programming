#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <ctype.h>
#include <unistd.h>
#include <uuid/uuid.h>
#include <errno.h>
#include <sys/stat.h>
#include <sys/param.h>
#include <getopt.h>
#include <dirent.h>
#include <fcntl.h>

int main()
{
    int fd = open("cp", O_WRONLY);
    printf("fd = %d\n", fd);

    printf("result of closing %d\n", close(fd));

    return 0;
}