#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <ctype.h>
#include <unistd.h>
#include <getopt.h>

#define BUFFSIZE 4096

const int FD_READ = 0;
const int FD_WRITE = 1;

const char flags [] = "+v";

struct option longopts[] = 
{
    { "verbose",       no_argument,      NULL,     'v' },
};

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

float gettime (struct timeval tp1, struct timeval tp2)
{
    long sec = tp2.tv_sec - tp1.tv_sec;
    long microsec = tp2.tv_usec - tp1.tv_usec;
    double TimePassed = (double)sec + ( (double)microsec ) / 1000000;

    return TimePassed;
}

int countLines(char* buffer, int Size)
{
    int nLines = 0;
    for (int i = 0; i < Size; i++)
    {
        if (buffer[i] == '\n')
        nLines++;
    }

    return nLines;
}

int countWords(char* buffer, int Size)
{
    int nWords = 0;
    int WordFlag = 0;
    for (int i = 0; i < Size; i++)
    {
        if (WordFlag)
        {
            if (isspace(buffer[i]))
            {
                WordFlag = 0;
            }
        }
        else
        {
            if (!isspace(buffer[i]))
            {
                WordFlag = 1;
                nWords++;
            }
        }
    }

    return nWords;
}



int main(int argc, char* argv[])
{
    if (argc < 1)
    {
        printf("You did not specify exec file");
    }
    
    int vFlag = 0;
    int newFlag;
    while ((newFlag = getopt_long (2, argv, flags, longopts, NULL)) != -1)
    if (newFlag == 'v')
    {
        vFlag = 1;
    }

    // printf("qflag = %d, optind = %d\n", qFlag, optind);
    // for (int i = 0; i < argc; i++)
    // {
    //     printf("%s\n", argv[i]);
    // }

    struct timeval tp1;
    gettimeofday(&tp1, NULL);

    int fd[2];

    int Error = pipe(fd);
    if(Error)
    {
        perror("problem with piping: ");
        return 0;
    }

    pid_t childPid = fork();
    if (childPid < 0)
    {
        perror("problem with forking: ");
        return 0;
    }
    else if(childPid == 0)
         {
            if (dup2(fd[FD_WRITE], 1) < 0)
                perror("There is a bit of problem with executing your programm");
            
            close(fd[FD_READ]);
            close(fd[FD_WRITE]);

            execvp(argv[optind], argv + optind);
            perror("There is a bit of problem with executing your programm");
         }
         else
         {
            close(fd[FD_WRITE]);

            if (!vFlag)
                close(FD_WRITE);

            int nBytes = 0;
            int nLines = 0;
            int nWords = 0;

            char buffer[BUFFSIZE];
            int ReadSize = 0;
            while ( (ReadSize = read(fd[0], buffer, BUFFSIZE)) != 0)
            {
                if (ReadSize < 0)
                {
                    perror("There appeared an error with reading");
                }

                if (vFlag)
                    MyWrite(1, buffer, ReadSize);

                nBytes += ReadSize;
                nLines += countLines(buffer, ReadSize);
                nWords += countWords(buffer, ReadSize);
            }

            close(fd[FD_WRITE]);
            fprintf(stderr, "Printing to stderr: nLines = %d, nWords = %d, nBytes = %d\n", nLines, nWords, nBytes);

            int status = 0;
            wait(&status);

            struct timeval tp2;
            gettimeofday(&tp2, NULL);
            double Time = gettime (tp1, tp2);

            fprintf(stderr, "Printing to stderr: Time passed = %lf\n",Time);
         }
}