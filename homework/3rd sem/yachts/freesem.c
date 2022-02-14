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
#include <sys/msg.h>
#include <sys/sem.h>
#include <sys/shm.h>

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

void Z( int sem_id, int ind )
{
    struct sembuf buf = {ind, 0, 0};
    semop(sem_id, &buf, 1);
}

int Z_NO_WAIT( int sem_id, int ind )
{
    struct sembuf buf = {ind, 0, IPC_NOWAIT};
    return semop(sem_id, &buf, 1);
}

int main()
{

    //printf("sizeof struct: %zu\n", sizeof(struct msgbuf));
    // int E = 327716;
    // int subjCond = 327717;
    // printf("results: %d and %d\n", msgctl(E, IPC_RMID, 0), msgctl(subjCond, IPC_RMID, 0));

    for (int i = 6422528; i < 6422529; i++)
    {
        printf("%d: result: %d\n", i, semctl(i, 0, IPC_RMID));
    }

    //semctl(semId, 0, IPC_RMID);
}