#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <uuid/uuid.h>
#include <errno.h>
#include <sys/param.h>
#include <fcntl.h>

#define MAX_SIZE 10

int copy (char* buffer, int size)
{
    int count_written = 0;
    
    do
    {
        count_written = write(1, buffer + count_written, size);   
    }while ((count_written != 0) && (count_written != size));

    return 0;
}

int mycat()
{
    char buffer[MAX_SIZE] = { 0 };

    int read_size = 0;

    while(1)
    {
        int read_size = read(0, buffer, MAX_SIZE);

        printf("\nread = %d\n", read_size);

        if (read_size < 0)
        {
            perror("There apperead a problem with reading the file: ");
            return (-1);
        }
        
        copy(buffer, read_size);

        if(read_size == 0)
        {
            printf("\nread_flag - 0\n");
            break;
        }
            

    }

    

    return 0;
}

int main(int argc, char* argv[])
{
    int flag = mycat();
    if (flag == -1)
    {
        printf("There appeared an error\n");
    }
}
