
#include <stdio.h>
#include <string.h>

int main(int argc, char *argv[])
{
    char flag = 0; // flag whether "-n" was used

    int i = 1;

    
    if (argc > 1)  // checking whether "-n" flag was used
        if ((strcmp(argv[1], "-n") == 0))
        {
            flag = 1;
            i++; 
        }

    for (i; i < argc; i++)
    {
        if (argc > 1 && (i != argc - 1) ) //checking whether to place a space after printing
            printf("%s ", argv[i]);
        else
            printf("%s", argv[i]);
    }
    
    if ( !flag )
        printf("\n");

    return 0;
}
