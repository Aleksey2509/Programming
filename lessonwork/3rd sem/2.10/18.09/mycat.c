#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <uuid/uuid.h>
#include <errno.h>
#include <sys/param.h>
#include <fcntl.h>

#define MAX_SIZE 1000

int copy (char* buffer, int size)
{
    int count_written = 0;
    
    do
    {
        count_written = write(1, buffer + count_written, size);   
    }while ((count_written != 0) && (count_written != size));

    return 0;
}

int mycat(int filecount, char* argv[])
{
    char buffer[MAX_SIZE] = {};

    for (int i = 0; i < filecount; i++)
    {

        //while(1)
        //{
            int fd = open(argv[i], O_RDONLY, 'r');

            if (fd < 0)
            {
                perror("There apperead a problem with opening the file: ");
                return (-1);
            }

            int read_flag = read(fd, buffer, MAX_SIZE);

            close(fd);

            if (read_flag < 0)
            {
                perror("There apperead a problem with reading the file: ");
                return (-1);
            }
            
            copy(buffer, read_flag);

            if (read_flag == 0)
                break;

            

        //}
    }

    

    return 0;
}

int main(int argc, char* argv[])
{
    mycat(argc - 1, argv + 1);
}
