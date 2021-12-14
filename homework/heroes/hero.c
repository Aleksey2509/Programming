#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <ctype.h>
#include <pwd.h>
#include <grp.h>
#include <unistd.h>
#include <uuid/uuid.h>
#include <errno.h>
#include <fcntl.h>
#include <time.h>
#include <sys/stat.h>
#include <sys/param.h>
#include <sys/types.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <sys/time.h>

enum Semaphores
{
    ACCESS_SEM = 0,
    CURRENT_CHAR_SEM,
    WAIT_FOR_NEXT,
    SLEEP_AMOUNT,
    WAKEN_UP
};

const char SetGreen[] = "\033[32m";
const char SetRed[] = "\033[31m";
const char SetDefault[] = "\033[0m";

struct CharInfo
{
    int occupiedChars;
    char takenBuf[60];
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

void Z( int semId, int semNum )
{
    struct sembuf operation = {semNum, 0, 0};
    semop(semId, &operation, 1);
}

int noWaitZ( int semId, int semNum )
{
    struct sembuf operation = {semNum, 0, IPC_NOWAIT};
    return semop(semId, &operation, 1);
}

int ifFirstToTake(struct CharInfo taken, char letter)
{
    int len = taken.occupiedChars;
    int flag = 1;
    for (int i = 0; i < len; i++)
    {
        if (taken.takenBuf[i] == letter)
        {
            flag = 0;
            break;
        }
    }

    return flag;
}



int child(int myPid, const char* toSing)
{
    key_t mykey = ftok("hero.c", 0);

    //printf("I am child %d, got key %d\n", myPid, mykey);
    int semId = semget(mykey, 5, IPC_CREAT | IPC_EXCL | 0777);

    if ((semId == -1) && (errno == EEXIST))
    {
        semId = semget(mykey, 5, 0777);
        //printf("child %d: got %d\n", myPid, semId);
    }
    else
    {
        //printf("got string %s, len - %lu, created %d\n", toSing, strlen(toSing), semId);
        V(semId, ACCESS_SEM, 1);
        int init = semctl(semId, ACCESS_SEM, GETVAL);
        printf("init = %d\n", init);
    }

    int shmId = shmget(mykey, sizeof(struct CharInfo), IPC_CREAT | IPC_EXCL | 0777);
    
    if ((shmId == -1) && (errno == EEXIST))
    {
        shmId = shmget(mykey, sizeof(struct CharInfo), 0777);
    }
    struct CharInfo* taken = shmat(shmId, NULL, 0);

    int len = strlen(toSing);
    int current = 0;
    int toSleep = 0;
    char myLetter = 0;
    int result;
    while (current <= (len - 1))
    {
        //printf("I am child %d, will try at a char\n", myPid);
        P(semId, ACCESS_SEM, 1);
        current = semctl(semId, CURRENT_CHAR_SEM, GETVAL);
        if (current == len)
        {
            V(semId, ACCESS_SEM, 1);
            break;
        }
        //printf("I am child %d, will look at a char %d\n", myPid, current);

        if ((myLetter == 0) && ifFirstToTake(*taken, toSing[current]))
        {
            myLetter = toSing[current];
            printf("I am bogatyr %02d; I am saying %s'%c'%s\n", myPid, SetGreen, myLetter, SetDefault);

            taken->takenBuf[taken->occupiedChars] = myLetter;
            (taken->occupiedChars)++;

            V(semId, CURRENT_CHAR_SEM, 1);
            current++;

            while(noWaitZ(semId, SLEEP_AMOUNT) != 0)
            {
                P(semId, SLEEP_AMOUNT, 1);
                V(semId, WAIT_FOR_NEXT, 1);
                //printf("child %d: sended a message to wake one\n", myPid);
                P(semId, WAKEN_UP, 1);
                //printf("child %d: waked one\n", myPid);
            }
        }
        else if (myLetter == toSing[current])
        {
            printf("I am bogatyr %02d; I am saying %s'%c'%s\n", myPid, SetGreen, myLetter, SetDefault);
            V(semId, CURRENT_CHAR_SEM, 1);
            current++;

            while(noWaitZ(semId, SLEEP_AMOUNT) != 0)
            {
                P(semId, SLEEP_AMOUNT, 1);
                V(semId, WAIT_FOR_NEXT, 1);
                //printf("child %d: sended a message to wake one\n", myPid);
                P(semId, WAKEN_UP, 1);
                //printf("child %d: waked one\n", myPid);
            }
        }
        else
        {
            printf("I am bogatyr %02d; my letter - '%c', but current - %s'%c'%s\n", myPid, myLetter, SetRed, toSing[current], SetDefault);
            toSleep = 1;
        }

        V(semId, ACCESS_SEM, 1);
        if (toSleep)
        {
            V(semId, SLEEP_AMOUNT, 1);
            //printf("child %d: going to wait for next char\n", myPid);
            P(semId, WAIT_FOR_NEXT, 1);
            V(semId, WAKEN_UP, 1);
            //printf("child %d: waked up for next char\n", myPid);
            toSleep = 0;
        }
        //printf("after = %d\n", semctl(semId, ACCESS_SEM, GETVAL));
    }

    //printf("child %d: returning\n", myPid);
    shmdt(taken);
    return 0;
}

int main()
{
    int childNum = 25;
    for (int i = 0; i < childNum; i++)
    {
        pid_t childPid = fork();
        if (childPid == 0)
        {
            child(i, "O great Heavens!");
            return 0;
        }
    }

    for (int i = 0; i < childNum; i++)
    {
        int status;
        wait(&status);
        //printf("waited for %d\n", i);
    }

    key_t mykey = ftok("hero.c", 0);
    int semId = semget(mykey, 4, 0777);
    semctl(semId, 0, IPC_RMID);
    int shmId = shmget(mykey, sizeof(struct CharInfo), 0777);
    shmctl(shmId, IPC_RMID, NULL);
}