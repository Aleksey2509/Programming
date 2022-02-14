#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>

int main (int argc, char* argv[], char* envp[])
{
    for (int i = 0; i < argc; i++)
    {
        printf("%s\n", argv[i]);
    }

    for (int j = 0; j < 30; j++)
    {
        printf("%s\n", envp[j]);
    }

    return 0;
}