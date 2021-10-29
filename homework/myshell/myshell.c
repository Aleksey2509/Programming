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

enum DUP_CONSTS
{
    FD_READ = 0,
    FD_WRITE = 1
};

const int PROGS_MAX = 20;
const int ARGS_MAX = 20;

#define BUFFSIZE 4096

enum ERRORS
{
    FORK_FAILURE = -1000,
    PIPE_FAILURE = -500,
    EXECUTION_FAILURE = -250,
    REALLOC_FAILURE = -125,
    OK = 0
};

struct PipeFd
{
    int fd[2];
};

pid_t myfork()
{
    pid_t childPid = fork();
    if (childPid < 0)
    {
        perror("problem with forking: ");
        exit (FORK_FAILURE);
    }
    return childPid;
}

int mypipe(int fd[2])
{
    int Error = pipe(fd);
    if(Error)
    {
        perror("problem with piping: ");
    }
    return OK;
}

char** parseIntoProgramms(char* cmdBuf, int* progNum)
{
    char** progPointer = (char**)calloc(PROGS_MAX, sizeof(char*));
    char* vertPtr = 0;

    int progCounter = 0;

    while ((vertPtr = strchr(cmdBuf, '|')) != 0)
    {
        //printf("HERE\n");
        progPointer[progCounter++] = vertPtr;
        if (progCounter == (PROGS_MAX - 1))
        {
            char** tmp = (char**)realloc(progPointer, 2 * PROGS_MAX * sizeof(char *));
                if (!tmp)
                {
                    *progNum = progCounter;
                    return NULL;
                }
                progPointer = tmp;
        }
        cmdBuf = vertPtr + 1;
    }
    *progNum = ++progCounter;

    return progPointer;
}

char** parseIntoArguments(char* cmdBuf, int* myArgc, char* argEnd)
{
    char** myArgv = (char**)calloc(ARGS_MAX, sizeof(char*));
    // printf("In func: allocated %p\n", *myArgv);
    // return 0;
    char* wordPtr = 0;
    int nWords = 0;

    int spaceNum = 0;
    char forSkip [BUFFSIZE] = { 0 };
    while(cmdBuf[0] == ' ')
    {
        cmdBuf++;
    }

    while ( ((cmdBuf < argEnd) || (argEnd == NULL)) && ((wordPtr = strchr(cmdBuf, ' ')) != 0) && ((wordPtr < argEnd) || (argEnd == NULL)) )
    {

        myArgv[nWords] = (char*)calloc((size_t)(wordPtr - cmdBuf + 1), sizeof(char));
        strncpy(myArgv[nWords], cmdBuf, ((size_t)wordPtr - (size_t)cmdBuf));
        //printf("got %s\n", myArgv[nWords]);
        nWords++;

        //printf("before: cmdbuf - %p, left in cmdbuf - %s, wordptr - %p\n", cmdBuf, cmdBuf, wordPtr); 
        cmdBuf = wordPtr + 1;
        //printf("after: cmdbuf - %p, left in cmdbuf - %s\n", cmdBuf, cmdBuf);

        while(cmdBuf[0] == ' ')
        {
            cmdBuf++;
        }

    }

    //printf("leftover - %s\n", cmdBuf);
    if (argEnd == NULL)
    {
        if (cmdBuf[0] != '\0')
        {
            myArgv[nWords] = strdup(cmdBuf);
            nWords++;
        }
    }
    else
    {
        if (cmdBuf[0] != '|')
        {
            myArgv[nWords] = strndup(cmdBuf, (unsigned long long)argEnd - (unsigned long long)cmdBuf);
            nWords++;
        }
    }
    

    *myArgc = nWords;

    return myArgv;
}

int closeFd (struct PipeFd* pipeArray, long long size, int childNum)
{
    //printf("\nin closefd(): childNum - %d, size - %lld\n", childNum, size);
    if (size < 0)
        return 0;

    for (int i = 0; i < size; i++)
    {
        close (pipeArray[i].fd[FD_READ]);
        close (pipeArray[i].fd[FD_WRITE]);
        //printf("closed %d and %d in process %d\n", pipeArray[i].fd[FD_READ], pipeArray[i].fd[FD_WRITE], childNum);
    }
    return 0;
}

int childProcess (int childNum, char* cmdBuf, char** argPointers, int progNum, struct PipeFd* pipeArray)
{
    if (childNum > 1)
        closeFd(pipeArray, childNum - 1, childNum);
    if (childNum < progNum - 2)
        closeFd(pipeArray + childNum + 1, progNum - (childNum + 2), childNum);

    //printf("childNum = %d, progNum = %d\n", childNum, progNum);
    int myArgc = 0;
    char** myArgv = 0;
    myArgv = parseIntoArguments(cmdBuf, &myArgc, argPointers[childNum]);

    //printf("prog number %d got %p", childNum, myArgv);

    //printf("got: %s, argcount - %d\n", cmdBuf, myArgc);

    // printf("printing words:\n");
    // for (int i = 0; i < myArgc; i++)
    //     printf("%s, len - %lu\n", myArgv[i], strlen(myArgv[i]));

    if (childNum > 0)
    {
        close (pipeArray[childNum - 1].fd[FD_WRITE]);
        //printf("pipe read end that will be used: %d\n", pipeArray[childNum - 1].fd[FD_READ]);
        if (dup2(pipeArray[childNum - 1].fd[FD_READ], FD_READ) < 0)
            fprintf(stderr, "There is a bit of problem with rerouting read end for process %d: %s", childNum, strerror(errno));
        close (pipeArray[childNum - 1].fd[FD_READ]);
    }
    else
    {
        close (pipeArray[0].fd[FD_READ]);
        //printf("closed the first pipe's read end with fd - %D\n", pipeArray[0].fd[FD_READ]);
    }

    if (childNum < (progNum - 1))
    {
        close (pipeArray[childNum].fd[FD_READ]);
        //printf("pipe write end that will be used: %d\n", pipeArray[childNum].fd[FD_WRITE]);
        if (dup2(pipeArray[childNum].fd[FD_WRITE], FD_WRITE) < 0)
            fprintf(stderr, "There is a bit of problem with rerouting write end for process %d: %s", childNum, strerror(errno));
        close (pipeArray[childNum].fd[FD_WRITE]);
    }
    else
    {
        close (pipeArray[childNum - 1].fd[FD_WRITE]);
    }

    execvp(myArgv[0], myArgv);

    perror("There is a bit of problem with executing your programm");
    free(myArgv);
    exit(EXECUTION_FAILURE);
    //break;
    //printf("Ended processing %d\n", progCount)
}

int myshell (int argc, char* argv[])
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
        char** argPointers;
        argPointers = parseIntoProgramms(cmdBuf, &progNum);

        // printf("got %d progs\n", progNum);
        // if (progNum > 0)
        // {
        //     printf("Printing ppointers\n");
        //     for (int i = 0; i < progNum; i++)
        //     {
        //         printf("pointer to prog %d: %p\n", i, argPointers[i]);
        //     }
        // }

        if ((argPointers == NULL) && (progNum > 1))
        {
            printf("Too much calling programms: cant get enough memory for their call\n");
            return REALLOC_FAILURE;
        }

        struct PipeFd* pipeArray = (struct PipeFd*) calloc(progNum - 1, sizeof(struct PipeFd));
        for (int pipeCount = 0; pipeCount < progNum - 1; pipeCount++)
        {
            int Error = pipe(pipeArray[pipeCount].fd);
            if (Error)
            {
                perror("problem with piping: ");
                free (argPointers);
                free (cmdBuf);
                continue;
            }
        }

        // for (int childNum = 0; childNum < progNum - 1; childNum++)
        // {
        //     printf("fd array num %d: read end: %d write end %d\n", childNum, pipeArray[childNum].fd[FD_READ], pipeArray[childNum].fd[FD_WRITE]);
        // }

        pid_t childPid = 0;
        int childNum = 0;
        for (childNum = 0; childNum < progNum; childNum++)
        {
            childPid = myfork();

            if(childPid == 0)
            {
                break;
            }

        }

        if (childPid == 0)
        {
            if (childNum == 0)
                childProcess(childNum, cmdBuf, argPointers, progNum, pipeArray);
            else
                childProcess(childNum, argPointers[childNum - 1] + 1, argPointers, progNum, pipeArray);
        }


        for (int j = 0; j < progNum - 1; j++)
        {
            close(pipeArray[j].fd[FD_WRITE]);
            close(pipeArray[j].fd[FD_READ]);
            //printf("closed %D and %d\n", pipeArray[j].fd[FD_WRITE], pipeArray[j].fd[FD_READ]);
        }

        for (int j = 0; j < progNum; j++)
        {
            int status;
            wait(&status);
            //printf("waited for %d child\n", j);
        }
        free(argPointers);
        free(cmdBuf);
    }

    return 0;
}

int main(int argc, char* argv[])
{
    myshell(argc, argv);
}