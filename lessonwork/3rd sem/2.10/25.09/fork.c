#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(int argc, char* argv[])
{
    int processNum = atoi(argv[1]);

    int status = 0;
    
    
    for (int i = 0; i < processNum; i++)
    {
        pid_t childpid = fork();

        if (childpid < 0)
        {
            printf("Error\n");
        }
        else if(childpid == 0)
            {
                printf("my parent pid = %d , my pid = %d, i = %d\n", getpid(), getppid(), i);
            }
            else
            {
                wait(&status);
                break;
            }
    }
            
}