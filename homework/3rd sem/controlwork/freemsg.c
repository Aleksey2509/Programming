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
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/time.h>
#include <sys/sem.h>

struct SubjectCondition
{
    int S;
    int B;
};

struct msgbuf
{
    long mtype;
    long needNew;
    struct SubjectCondition subjCond;
};

int main()
{
    //printf("sizeof struct: %zu\n", sizeof(struct msgbuf));
    // int E = 327716;
    // int subjCond = 327717;
    // printf("results: %d and %d\n", msgctl(E, IPC_RMID, 0), msgctl(subjCond, IPC_RMID, 0));

    for (int i = 2752515; i < 2752516; i++)
    {
        printf("%d: result: %d\n", i, msgctl(i, IPC_RMID, 0));
    }
    printf("result: %d\n", msgctl(1310724, IPC_RMID, 0));
    printf("result: %d\n", msgctl(2949122, IPC_RMID, 0));
}