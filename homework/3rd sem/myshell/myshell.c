#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <errno.h>
#include <ctype.h>
#include <unistd.h>
#include <getopt.h>
#include <readline/readline.h>
#include <readline/history.h>

#define BUFFSIZE 4096

enum DUP_CONSTS
{
    FD_READ = 0,
    FD_WRITE = 1
};

enum ERRORS
{
    FORK_FAILURE = -1000,
    PIPE_FAILURE = -500,
    EXECUTION_FAILURE = -250,
    REALLOC_FAILURE = -125,
};

const int PROGS_MAX = 20;
const int ARGS_MAX = 20;

//-----------------------------------------------------------------------------------------------------------------------

pid_t myfork()
{
    pid_t childPid = fork();
    if (childPid < 0)
    {
        perror("problem with forking: ");
        return FORK_FAILURE;
    }
    return childPid;
}

int mypipe(int fd[2])
{
    int Error = pipe(fd);
    if(Error)
    {
        perror("problem with piping: ");
        return PIPE_FAILURE;
    }
    return 0;
}

char** parseIntoProgramms(char* cmdBuf, int* progNum)
{
    char** progPtr = (char**)calloc(PROGS_MAX, sizeof(char*));
    int progsAllocated = PROGS_MAX;
    char* progStr, *savePtr;
    int progCounter = 0;
    const char* delim = "|";
    progPtr[progCounter++] = cmdBuf;

    for(progStr = strtok_r(cmdBuf, delim, &savePtr); (progStr != NULL); progStr = strtok_r(NULL, delim, &savePtr))
    {
        progPtr[progCounter++] = progStr;
        if (progCounter == (progsAllocated - 1))
        {
            char** tmp = (char**)realloc(progPtr, 2 * progsAllocated * sizeof(char *));
                if (!tmp)
                {
                    *progNum = progCounter;
                    return NULL;
                }
                progsAllocated *= 2;
                progPtr = tmp;
        }

    }

    *progNum = progCounter;

    return progPtr;
}

//-----------------------------------------------------------------------------------------------------------------------

char** parseIntoArguments(char* argBuf, int* argc)
{
    //printf("argstart - %s, argend - %s\n", argBuf, argEnd);
    char** argv = (char**)calloc(ARGS_MAX, sizeof(char*));
    int argsAllocated = ARGS_MAX;
    int nWords = 0;
    char* wordPtr, *savePtr;
    char delim[] = " |";

    for(wordPtr = strtok_r(argBuf, delim, &savePtr); (wordPtr != NULL); wordPtr = strtok_r(NULL, delim, &savePtr))
    {
        //printf("got a word - %s.\n", wordPtr);
        argv[nWords++] = wordPtr;

        if (nWords == (argsAllocated - 1))
        {
            char** tmp = (char**)realloc(argv, 2 * argsAllocated * sizeof(char *));
                if (!tmp)
                {
                    *argc = nWords;
                    return NULL;
                }
                argsAllocated *= 2;
                argv = tmp;
        }

    }

    *argc = nWords;
    return argv;
}

//-----------------------------------------------------------------------------------------------------------------------

int childProcess (int childNum, char* cmdBuf, int progNum, int fd[2])
{
    //printf("childNum = %d, progNum = %d\n", childNum, progNum);
    int childArgc = 0;
    char** childArgv = parseIntoArguments(cmdBuf, &childArgc);

    // for (int i = 0; i < childArgc; i++)
    //     printf("arg %d - %s\n", i + 1, childArgv[i]);

    if (fd[FD_READ] != FD_READ)
    {
        dup2(fd[FD_READ], FD_READ);
        close (fd[FD_READ]);
    }

    if (fd[FD_WRITE] != FD_WRITE)
    {
        dup2(fd[FD_WRITE], FD_WRITE);
        close (fd[FD_WRITE]);
    }

    execvp(childArgv[0], childArgv);

    printf("There is a bit of problem with executing programm: %s: %s\n", childArgv[0], strerror(errno));

    free(childArgv);
    exit(EXECUTION_FAILURE);

}

//-----------------------------------------------------------------------------------------------------------------------

int myshell ()
{
    const char Prompt[] = "->";

    while (1)
    {
        char* cmdBuf = readline(Prompt);
        //printf("got %s\n", cmdBuf);
        if (cmdBuf == NULL)
        {
            printf("\nlogout\n");
            break;
        }

        int progNum = 0;
        char** argPointers = parseIntoProgramms(cmdBuf, &progNum);

        if ((argPointers == NULL) && (progNum > 1))
        {
            printf("Too much calling programms: cant get enough memory for their call\n");
            return REALLOC_FAILURE;
        }

        int readFd = 0;
        for (int childNum = 0; childNum < progNum; childNum++)
        {
            int fd[2];
            if (childNum != (progNum - 1))
                mypipe(fd);
            else
            {
                fd[FD_READ] = readFd;
                fd[FD_WRITE] = FD_WRITE;
            }

            pid_t childPid = myfork();
            if (childPid < 0)
            {
                fprintf(stderr, "Problems with fork: %s", strerror(errno));
            }
            else if (childPid == 0)
            {
                if (childNum != (progNum - 1))
                {
                    close(fd[FD_READ]);
                    fd[FD_READ] = readFd;
                }

                childProcess(childNum, argPointers[childNum], progNum, fd);
            }
            else
            {
                if (readFd != FD_READ)
                    close(readFd);
                if (fd[FD_WRITE] != FD_WRITE)
                    close(fd[FD_WRITE]);

                readFd = fd[FD_READ];
            }
        }

        for (int j = 0; j < progNum; j++)
        {
            int status;
            wait(&status);
        }

        free(argPointers);
        free(cmdBuf);
    }

    return 0;
}

//-----------------------------------------------------------------------------------------------------------------------

int main(int argc, char* argv[])
{
    myshell();

    return 0;
}