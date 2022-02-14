#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <ctype.h>
#include <unistd.h>
#include <uuid/uuid.h>
#include <errno.h>
#include <sys/stat.h>
#include <sys/param.h>
#include <getopt.h>
#include <dirent.h>
#include <fcntl.h>
#include <sys/ipc.h>
#include <sys/sem.h>

enum Semaphores
{
    ACCESS_SEM = 0, // semaphore to ensure only one bogatyr can look at current character at the same time
    PROG_COUNT
};

void P( int semId, int semNum, short value )
{
    struct sembuf operation = {semNum, -value, 0};
    semop(semId, &operation, 1);
}

void V( int semId, int semNum, short value )
{
    struct sembuf operation = {semNum, value, 0};
    semop(semId, &operation, 1);
}

int introvert()
{
    key_t mykey = ftok("introvert.c", 0);
    char Hello[] = "Hello world!\n";
    char GoodBye[] = "GoodBye!\n";
    int semId = semget(mykey, 2, IPC_CREAT | IPC_EXCL | 0777);

    if ((semId == -1) && (errno == EEXIST))
    {
        semId = semget(mykey, 2, 0777);
        //printf("child %d: got %d\n", myPid, semId);
    }
    else
    {
        //printf("got string %s, len - %lu, created %d\n", toSing, strlen(toSing), semId);
        V(semId, ACCESS_SEM, 1);
        int init = semctl(semId, ACCESS_SEM, GETVAL);
    }

    P(semId, ACCESS_SEM, 1);
    int progCount = semctl(semId, PROG_COUNT, GETVAL);
    V(semId, PROG_COUNT, 1);
    V(semId, ACCESS_SEM, 1);

    progCount = semctl(semId, PROG_COUNT, GETVAL);
    if (progCount == 0)
    {
        int len = strlen(Hello);
        for (int i = 0; i < len; i++)
        {
            sleep(1);
            printf("%c", Hello[i]);
        }
        printf("\n");
        
    }
    else
    {
        int len = strlen(GoodBye);
        for (int i = 0; i < len; i++)
        {
            sleep(1);
            printf("%c", GoodBye[i]);
        }
        printf("\n");
    }

    P(semId, ACCESS_SEM, 1);
    P(semId, PROG_COUNT, 1);
    V(semId, ACCESS_SEM, 1);

    return 0;
}

int main()
{
    introvert();

    key_t mykey = ftok("introvert.c", 0);
    int semId = semget(mykey, 2, 0777);

    if ((semId == -1) && (errno == EEXIST))
    {
        semId = semget(mykey, 2, 0777);
        //printf("child %d: got %d\n", myPid, semId);
    }
    else
    {
        //printf("got string %s, len - %lu, created %d\n", toSing, strlen(toSing), semId);
        V(semId, ACCESS_SEM, 1);
        int init = semctl(semId, ACCESS_SEM, GETVAL);
    }

    P(semId, ACCESS_SEM, 1);
    int progCount = semctl(semId, PROG_COUNT, GETVAL);
    if (progCount == 0)
        semctl(semId, 0, IPC_RMID);
    else
        V(semId, ACCESS_SEM, 1);

    return 0;
}