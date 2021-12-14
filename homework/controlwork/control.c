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

#define SCREW_MAN 1
#define BOLT_MAN 2
#define PLACE_NEW 3

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

void printmsg(int workerId, struct msgbuf toPrint)
{
    printf("Called from worker %d", workerId);
    printf(" msg: type - %ld needNew - %ld, condition of screws - %d, condition of bolt - %d\n", toPrint.mtype, toPrint.needNew, toPrint.subjCond.S, toPrint.subjCond.B);
}

int placeScrew()
{
    printf("Hey, i am a screw man, i am placing screw\n");
    return 0;
}

int placeBolt()
{
    printf("Hey, i am a bolt man, i am placing bolt\n");
    return 0;
}

int removeSubj(int workerId)
{
    switch (workerId)
    {
        case SCREW_MAN: printf("Hey, i am a screw man, i am removing subject\n");
                        break;
        case BOLT_MAN: printf("Hey, i am a bolt man, i am removing subject\n");
                        break;
    }

    return 0;
}

int placeNew(int workerId)
{
    switch (workerId)
    {
        case SCREW_MAN: printf("Hey, i am a screw man, i am placing new subject\n");
                        break;
        case BOLT_MAN: printf("Hey, i am a bolt man, i am placing new subject\n");
                        break;
    }

    return 0;
}

//-______--______--______--______--______--______--______--______--______--______--______--______--______--______--______--______-

int screwMan(int num, int N, int E, int subjCond, int S, int B)
{
    printf("Hey, i am a screw man %d\n", num);
    struct msgbuf condition = {1, 0, 0, 0};
    struct msgbuf needNew;
    int msgSize = sizeof(struct msgbuf) - sizeof(long);

    for (int subjNum = 1; ;)
    {

        int toPlace = msgrcv(E, &needNew, msgSize, 0, IPC_NOWAIT);
        //printf("toPlace - %d\n", toPlace);
        if (toPlace >= 0)
        {
            placeNew(SCREW_MAN);

            needNew.needNew = 0;
            condition = needNew;

            msgsnd(subjCond, &condition, msgSize, 0);
        }
        msgrcv(subjCond, &condition, msgSize, 0, 0);

        subjNum = condition.mtype;

        printf("Screw man %d, working on %d subj\n", num, subjNum);

        if (condition.subjCond.S != 2)
        {
            placeScrew();
            condition.subjCond.S++;
            msgsnd(subjCond, &condition, msgSize, 0);
        }
        else
        {
            if (condition.subjCond.B == 1)
            {
                removeSubj(SCREW_MAN);
                if (subjNum == N)
                {
                    msgsnd(subjCond, &condition, msgSize, 0);
                    printf("screw man %d; ended my work for today\n", num);
                    return 0;
                }
                needNew.mtype = subjNum + 1;
                needNew.needNew = 1;

                msgsnd(E, &needNew, 0, 0);

                long tmp = subjNum + 1;
                msgsnd(B, &tmp, 0, 0);
                msgsnd(S, &tmp, 0, 0);
            }
            else
            {
                printf("Screw man %d; ended my work on %d subject\n", num, subjNum);
                msgsnd(subjCond, &condition, msgSize, 0);
                if (subjNum == N)
                {
                    printf("screw man %d; ended my work for today\n", num);
                    return 0;
                }
                long tmp;
                msgrcv(S, &tmp, 0, 0, 0);
            }
        }

    }

    return 0;
}

//-______--______--______--______--______--______--______--______--______--______--______--______--______--______--______--______-

int boltMan(int N, int E, int subjCond, int S, int B)
{
    printf("Hey, i am a bolt man\n");
    struct msgbuf condition = {1, 0, 0, 0};
    struct msgbuf needNew;
    int msgSize = sizeof(struct msgbuf) - sizeof(long);
    //printf("msgSize - %d\n", msgSize);

    for (int subjNum = 1; ;)
    {
        int toPlace = msgrcv(E, &needNew, msgSize, 0, IPC_NOWAIT);
        //printf("toPlace - %d\n", toPlace);
        if (toPlace >= 0)
        {
            placeNew(BOLT_MAN);

            needNew.needNew = 0;
            condition = needNew;

            msgsnd(subjCond, &condition, msgSize, 0);
        }
        msgrcv(subjCond, &condition, msgSize, 0, 0);

        subjNum = condition.mtype;
        printf("Bolt man, working on %d subj\n", subjNum);
        //printmsg(BOLT_MAN, needNew);

        placeBolt();
        condition.subjCond.B++;

        if (condition.subjCond.S == 2)
        {
            removeSubj(BOLT_MAN);
            if (subjNum == N)
            {
                printf("bolt man; ended my work for today\n");
                return 0;
            }
            needNew.mtype = subjNum + 1;
            needNew.needNew = 1;

            msgsnd(E, &needNew, msgSize, 0);

            long tmp = subjNum + 1;
            msgsnd(S, &tmp, 0, 0);
            msgsnd(S, &tmp, 0, 0);
        }
        else
        {
            printf("bolt man; ended my work on %d subject\n", subjNum);
            msgsnd(subjCond, &condition, msgSize, 0);
            if (subjNum == N)
            {
                printf("bolt man; ended my work for today\n");
                return 0;
            }
            long tmp;
            msgrcv(B, &tmp, 0, 0, 0);
        }
    }

    return 0;
}

int main(int argc, char* argv[])
{
    int subjCond = msgget(IPC_PRIVATE, 0777);
    int E = msgget(IPC_PRIVATE, 0777);
    int S = msgget(IPC_PRIVATE, 0777);
    int B = msgget(IPC_PRIVATE, 0777);
    //printf("E - %d, subjcond - %d\n", E, subjCond);
    //printf("main sending a init message\n");
    struct msgbuf init = {1, 1, 0, 0};
    //printmsg(0, init);
    msgsnd(E, &init, sizeof(init) - sizeof(long), 0);

    int N;
    if (argc > 1)
        N = atoi(argv[1]);
    else
        N = 5;

    int i;
    pid_t childPid;
    for (i = 0; i < 3; i++)
    {
        childPid = fork();

        if (childPid == 0)
        {
            if (i < 2)
            {
                printf("Calling screw man\n");
                screwMan(i, N, E, subjCond, S, B);
                return 0;
            }
            else
            {
                printf("Calling bolt man\n");
                boltMan(N, E, subjCond, S, B);
                return 0;
            }
        }
    }

    for (i = 0; i < 3; i++)
    {
        int status;
        wait(&status);
    }

    printf("main waited for children and freeing msg queues\n");

    msgctl(E, IPC_RMID, 0);
    msgctl(subjCond, IPC_RMID, 0);
    msgctl(S, IPC_RMID, 0);
    msgctl(B, IPC_RMID, 0);
}