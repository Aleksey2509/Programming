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
#include <sys/time.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>

struct CharInfo
{
    int occupiedChars;
    char takenBuf[60];
};

void P( int sem_id, int ind, short value )
{
    struct sembuf buf = {ind, -value, 0};
    semop(sem_id, &buf, 1);
}

void V( int sem_id, int ind, short value )
{
    struct sembuf buf = {ind, value, 0};
    semop(sem_id, &buf, 1);
}

int main()
{
    int semId = semget(IPC_PRIVATE, 1, 0);
    V(semId, 0, 1);

    pid_t childPid = fork();

    key_t mykey = ftok("shmtest.c", 0);
    int shmId = shmget(mykey, sizeof(struct CharInfo), IPC_CREAT | IPC_EXCL | 0777);
    
    if ((shmId == -1) && (errno == EEXIST))
    {
        shmId = shmget(mykey, sizeof(struct CharInfo), 0777);
    }

    struct CharInfo* taken = shmat(shmId, NULL, 0);

    if (!childPid)
    {
        sleep(1);
        P(semId, 0, 1);
        taken->takenBuf[taken->occupiedChars] = 'a';
        taken->occupiedChars++;
        V(semId, 0, 1);
    }
    else
    {
        P(semId, 0, 1);
        taken->takenBuf[taken->occupiedChars] = 'b';
        taken->occupiedChars++;
        V(semId, 0, 1);
    }

    if (childPid)
    {
        sleep(2);
        P(semId, 0, 1);
        printf("occupied chars - %d\n", taken->occupiedChars);
        for (int i = 0; i < taken->occupiedChars; i++)
            printf("%d - %c\n", i, taken->takenBuf[i]);
        V(semId, 0, 1);
    }

    shmdt(taken);

    if (childPid)
    {
        sleep(2);
        shmctl(shmId, IPC_RMID, NULL);
        semctl(semId, 0, IPC_RMID);
    }
}