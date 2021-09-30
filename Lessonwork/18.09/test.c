#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <uuid/uuid.h>
#include <errno.h>
#include <sys/param.h>
#include <fcntl.h>

#define MAX_SIZE 1000

int writeToStdout (char* buffer, int size)
{
    int countWritten = 0;
    int LeftToWrite = 0;
    
    do
    {
        countWritten = write(1, buffer + countWritten, size);
        LeftToWrite = size - countWritten;
    }while ((countWritten != 0) && (LeftToWrite != 0));

    return 0;
}

catFromStdin(buffer)
{
    int read_size = 0;

    while(1)
    {
        int read_size = read(0, buffer, MAX_SIZE);

        if (read_size < 0)
        {
            perror("There apperead a problem with reading the file: ");
            return -1;
        }
        
        copy(buffer, read_size);

        if(read_size == 0)
            break;
            

    }

    return 0;
}

catFromFile(int filecount, char* argv[], char* buffer)
{
    for (int i = 0; i < filecount; i++)
    {
        // while (1)
        // {
            int fd = open(fd, O_RDONLY, 'r');

            if (fd < 0)
            {
                printf("\n\nWarning! File #%d\n\n", i + 1);
                perror("There apperead a problem with opening the file");
                close(fd);
                continue;
            }

            int read_flag = read(fd, buffer, MAX_SIZE);

            if (read_flag < 0)
            {
                perror("There apperead a problem with reading the file: ");
                continue;
            }
            
            writeToStdout(buffer, read_flag);

            // if(read_flag == 0)
            //     break;
        // }

            

    }
}



int mycat(int filecount, char* argv[])
{
    
    char buffer[MAX_SIZE] = {};

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
