#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <errno.h>
#include <ctype.h>
#include <unistd.h>
#include <getopt.h>
#include <poll.h>

#define BUFFSIZE 4096

const char flags [] = "n";

enum DUP_CONSTS
{
    FD_READ = 0,
    FD_WRITE = 1
};

pid_t myfork()
{
    pid_t childPid = fork();
    if (childPid < 0)
    {
        perror("problem with forking: ");
        exit(-1);
    }
    return childPid;
}

int mypipe(int fd[2])
{
    int Error = pipe(fd);
    if(Error)
    {
        perror("problem with piping: ");
        exit(-1);
    }
    return 0;
}

int myWrite (int fd, char* buffer, int size)
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
    }while (LeftToWrite != 0);

    return 0;
}

int readWrite (int fdRead, int fdWrite)
{
    char buffer[BUFFSIZE] = { 0 };
    int readSize;
    while(1)
    {
        readSize = read(fdRead, buffer, BUFSIZ);
        if (readSize < 0)
        {
            return -1;
        }

        int error = myWrite(fdWrite, buffer, readSize);
        if(error < 0)
        {
            return -1;
        }

        if(readSize == 0)
            break;
    }

    return readSize;
}



int childProcess(int childNum, int* inputFd[2], int* outputFd[2], int nProc)
{
    for (int i = 0; i < childNum; i++)
    {
        close(inputFd[i][FD_READ]);
        close(outputFd[i][FD_READ]);
        close(inputFd[i][FD_WRITE]);
        close(outputFd[i][FD_WRITE]);
    }
    close(inputFd[childNum][FD_WRITE]);
    close(outputFd[childNum][FD_READ]);

    readWrite(inputFd[childNum][FD_READ], outputFd[childNum][FD_WRITE]);

    close(inputFd[childNum][FD_READ]);
    close(outputFd[childNum][FD_WRITE]);

    return 0;
}



int main(int argc, char* argv[])
{
    int newFlag, n = 0;

    struct option longopts[] = 
    {
        { "num",       required_argument,      &n,     'n' },
    };

    while ((newFlag = getopt_long (argc, argv, flags, longopts, NULL)) != -1)

    if (n == 0)
        n = 10;

    printf("n = %d\n", n);

    int** inputFd = calloc(n, sizeof(int*));
    int** outputFd = calloc(n, sizeof(int*));
    for (int i = 0; i < n; i++)
    {
        inputFd[i] = calloc(2, sizeof(int));
        outputFd[i] = calloc(2, sizeof(int));
    }

    for (int childNum = 0; childNum < n; childNum++)
        {
            mypipe(inputFd[childNum]);
            mypipe(outputFd[childNum]);

            pid_t childPid = myfork();
            if (childPid < 0)
            {
                fprintf(stderr, "Problems with fork: %s", strerror(errno));

            }
            else if (childPid == 0)
            {
                childProcess(childNum, inputFd, outputFd, n);
                return 0;
            }
        }



    char** buffArr = calloc(n, BUFFSIZE);
    for (int i = 0; i < n; i++)
    {
        buffArr[i] = calloc(BUFFSIZE, 1);
    }

    struct pollfd* fds = calloc(2 * n, sizeof(struct pollfd));

    for (int i = 0; i < n; i++)
    {
        fds[i].fd = outputFd[i][FD_READ];
        fds[i].events = POLLIN | POLLERR | POLLNVAL;
    }

    for (int i = n; i < 2 * n; i++)
    {
        fds[i].fd = inputFd[i - n][FD_WRITE];
        fds[i].events = POLLOUT | POLLERR;
    }

    if (n == 0)
    {
        readWrite(FD_READ, FD_WRITE);
    }
    else
    {

        int finishedChilds;
        while(finishedChilds != n)
        {
            char inputBuffer[BUFFSIZE]; 
            int inputReadSize = read(FD_READ, inputBuffer, BUFFSIZE);

            int status = poll(fds, n, 0);
            if (status < 0)
            {
                perror("An error with poll occured");
                exit(1);
            }

            finishedChilds = 0;
            for (int childNum = 0; childNum < n; childNum++)
            {
                if(fds[childNum].revents & POLLNVAL)
                {
                    finishedChilds++;
                    continue;
                }

                if (fds[childNum].revents & POLLERR || fds[childNum + n].revents & POLLERR)
                {
                    fprintf(stderr, "An error with %d child occured\n: %s", childNum, strerror(errno));
                    return -1;
                }

                int readSize;
                if(fds[childNum].revents & POLLIN)
                {
                    readSize = read(outputFd[childNum][FD_READ], buffArr[childNum], BUFSIZ);
                    if (readSize < 0)
                    {
                        fprintf(stderr, "The following problem appeared with child %d\n", childNum);
                        return -1;
                    }
                }

                if (fds[childNum + n].revents & POLLOUT)
                {
                    if (childNum == 0)
                    {
                        int error = myWrite(inputFd[childNum][FD_WRITE], inputBuffer, inputReadSize);
                        if(error < 0)
                        {
                            fprintf(stderr, "The following problem appeared with child %d\n", childNum);
                            return -1;
                        }
                    }

                    else if (childNum != n - 1)
                    {
                        int error = myWrite(inputFd[childNum + 1][FD_WRITE], buffArr[childNum], readSize);
                        if(error < 0)
                        {
                            fprintf(stderr, "The following problem appeared with child %d\n", childNum);
                            return -1;
                        }
                    }

                    else
                    {
                        int error = myWrite(FD_WRITE, buffArr[childNum], readSize);
                        if(error < 0)
                        {
                            // printf("The mentioned problem appeared with file %s\n", argv[i]);
                            return -1;
                        }
                    }

                }
            }
        }
    }


    for (int i = 0; i < n; i++)
    {
        int status;
        wait(&status);
    }

    for (int i = 0; i < n; i++)
    {
        close(inputFd[i][FD_READ]);
        close(outputFd[i][FD_READ]);
        close(inputFd[i][FD_WRITE]);
        close(outputFd[i][FD_WRITE]);
    }

    return 0;
}