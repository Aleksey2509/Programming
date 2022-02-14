#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(int argc, char* argv[])
{
    int* array = (int*) calloc(argc - 1, sizeof(int));

    for (int i = 1; i < argc; i++)
    {
        array[i - 1] = atoi(argv[i]);
    }

    pid_t childpid;

    for (int i = 0; i < argc - 1; i++)
    {
        childpid = fork();

        if (childpid < 0)
        {
            printf("Error forking\n");
        }
        else if(childpid == 0)
            {
                usleep(100 * array[i]);

                printf("%d ", array[i]);
                break;
            }
    }

    for (int i = 0; i < argc - 1; i++)
    {
        int status = 0;
        wait(&status);
    }

    if (childpid != 0)
    {
        free(array);
        printf("\n");
    }
        
}