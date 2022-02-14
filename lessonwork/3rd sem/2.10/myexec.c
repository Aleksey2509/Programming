#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <unistd.h>


int printtime (struct timeval tp1, struct timeval tp2)
{
    long sec = tp2.tv_sec - tp1.tv_sec;
    long microsec = tp2.tv_usec - tp1.tv_usec;
    double TimePassed = (double)sec + ( (double)microsec ) / 1000000;

    printf("%lf\n", TimePassed);
    return 0;
}

int main(int argc, char* argv[])
{
    if (argc < 1)
    {
        printf("You did not specify exec file");
    }

    struct timeval tp1;
    gettimeofday(&tp1, NULL);

    pid_t childPid = fork();
    
    if (childPid < 0)
    {
        printf("Error\n");
    }
    else if(childPid == 0)
        {
            execvp(argv[1], argv + 1);
            perror("Could not find exec file or not enough args");
        }
        else
        {
            int status = 0;
            wait(&status);
            printf("WHAT\n");

            struct timeval tp2;
            gettimeofday(&tp2, NULL);

            printtime (tp1, tp2);
        }
}       
