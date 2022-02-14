#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <uuid/uuid.h>
#include <errno.h>
#include <sys/param.h>
#include <fcntl.h>

const int FD_STDIN = 0;
const int FD_STDOUT = 1;

int MyWrite (int fd, char* buffer, int size)
{
    int countWritten = 0;
    int LeftToWrite = size;
    
    do
    {
        countWritten = write(fd, buffer + countWritten, size);

        if (countWritten < 0)
        {
            perror("There appeared problem with writing");
            return -1;
        }

        LeftToWrite -= countWritten;
    }while ((countWritten != 0) && (LeftToWrite != 0));

    return 0;
}

int MyRead (int fd, char* buffer, int size)
{
    int ReadSize = read(fd, buffer, BUFSIZ);

    if (ReadSize < 0)
    {
        perror("There apperead a problem with reading: ");
        return -1;
    }

    return ReadSize;
}

int catFromStdin(char* buffer)
{
    while(1)
    {
        int ReadSize = MyRead(FD_STDIN, buffer, BUFSIZ);
        if (ReadSize == -1)
            return -1;

        int Error = MyWrite(FD_STDOUT, buffer, ReadSize);
        if(Error == -1)
            return -1;

        if(ReadSize == 0)
            break;
    }

    return 0;
}

int catFromFile(int filecount, char* argv[], char* buffer)
{
    for (int i = 0; i < filecount; i++)
    {
        int fd = open(argv[i], O_RDONLY, 'r');

            if (fd < 0)
            {
                printf("mycat: %s: %s\n", argv[i], strerror(errno));
                close(fd);
                continue;
            }
        
        while (1)
        {
            int ReadSize = MyRead(fd, buffer, BUFSIZ);
            if (ReadSize == -1)
            {
                printf("The mentioned problem appeared with file %s\n", argv[i]);
                return -1;
            }

            int Error = MyWrite(FD_STDOUT, buffer, ReadSize);
            if(Error == -1)
            {
                printf("The mentioned problem appeared with file %s\n", argv[i]);
                return -1;
            }

            if(ReadSize == 0)
                break;
        }

        close(fd);

    }

    return 0;
}



int mycat(int filecount, char* argv[])
{
    
    char buffer[BUFSIZ] = {};

    switch(filecount)
    {
        case 0: catFromStdin(buffer);
                break;
        default: catFromFile(filecount, argv, buffer);
                 break;
    }

    return 0;
}

int main(int argc, char* argv[])
{
    mycat(argc - 1, argv + 1);
}
