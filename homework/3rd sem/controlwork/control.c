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

#define FIRST_NUT_MAN 1
#define SECOND_NUT_MAN 2
#define SCREW_MAN 3

struct SubjectCondition
{
    int N;
    int S;
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
    printf(" msg: type - %ld needNew - %ld, condition of nuts - %d, condition of screw - %d\n", toPrint.mtype, toPrint.needNew, toPrint.subjCond.N, toPrint.subjCond.S);
}

int placeNut(int workerId)
{
    printf("Hey, i am a nut man %d, i am placing nut\n", workerId);
    return 0;
}

int placeScrew()
{
    printf("Hey, i am a screw man, i am placing screw\n");
    return 0;
}

int removeSubj(int workerId)
{
    switch (workerId)
    {
        case FIRST_NUT_MAN: printf("Hey, i am a nut man %d, i am removing subject\n", workerId);
                        break;
        case SECOND_NUT_MAN: printf("Hey, i am a nut man %d, i am removing subject\n", workerId);
                        break;
        case SCREW_MAN: printf("Hey, i am a screw man, i am removing subject\n");
                        break;
    }

    return 0;
}

int placeNew(int workerId)
{
    switch (workerId)
    {
        case FIRST_NUT_MAN: printf("Hey, i am a nut man %d, i am placing new subject\n", workerId);
                        break;
        case SECOND_NUT_MAN: printf("Hey, i am a nut man %d, i am placing new subject\n", workerId);
                        break;
        case SCREW_MAN: printf("Hey, i am a screw man, i am placing new subject\n");
                        break;
    }

    return 0;
}

//-______--______--______--______--______--______--______--______--______--______--______--______--______--______--______--______-

int nutMan(int workerId, int amount, int E, int subjCond, int N, int S)
{
    printf("Hey, i am a nut man %d\n", workerId);
    struct msgbuf condition = {1, 0, 0, 0};
    struct msgbuf needNew;
    int msgSize = sizeof(struct msgbuf) - sizeof(long);

    for (int subjNum = 1; ;)
    {
        // if (workerId == 2)
        //    sleep(2);
        int toPlace = msgrcv(E, &needNew, msgSize, 0, IPC_NOWAIT);
        //printf("toPlace - %d\n", toPlace);
        if (toPlace >= 0)
        {
            placeNew(workerId);

            needNew.needNew = 0;
            condition = needNew;

            msgsnd(subjCond, &condition, msgSize, 0);
        }
        printf("Nut man %d, trying to get info what we working on\n", workerId);
        msgrcv(subjCond, &condition, msgSize, 0, 0);

        subjNum = condition.mtype;
        printf("Nut man %d, working on %d subj\n", workerId, subjNum);

        if (condition.subjCond.N != 2)
        {
            placeNut(workerId);
            condition.subjCond.N++;
        }

        if (condition.subjCond.N != 2)
            msgsnd(subjCond, &condition, msgSize, 0);
        else
        {
            if (condition.subjCond.S == 1)
            {
                removeSubj(workerId);
                if (subjNum == amount)
                {
                    msgsnd(subjCond, &condition, msgSize, 0);
                    printf("nut man %d; ended my work for today\n", workerId);
                    return 0;
                }
                needNew.mtype = subjNum + 1;
                needNew.needNew = 1;

                msgsnd(E, &needNew, 0, 0);

                long tmp = subjNum + 1;
                msgsnd(N, &tmp, 0, 0);
                msgsnd(S, &tmp, 0, 0);
            }
            else
            {
                printf("Nut man %d; ended my work on %d subject\n", workerId, subjNum);
                msgsnd(subjCond, &condition, msgSize, 0);
                if (subjNum == amount)
                {
                    printf("Nut man %d; ended my work for today\n", workerId);
                    return 0;
                }
                long tmp;
                msgrcv(N, &tmp, 0, 0, 0);
            }
        }

    }

    return 0;
}

//-______--______--______--______--______--______--______--______--______--______--______--______--______--______--______--______-

int screwMan(int amount, int E, int subjCond, int N, int S)
{
    printf("Hey, i am a screw man\n");
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
            placeNew(SCREW_MAN);

            needNew.needNew = 0;
            condition = needNew;

            msgsnd(subjCond, &condition, msgSize, 0);
        }
        msgrcv(subjCond, &condition, msgSize, 0, 0);

        subjNum = condition.mtype;
        printf("Screw man, working on %d subj\n", subjNum);
        //printmsg(BOLT_MAN, needNew);

        placeScrew();
        condition.subjCond.S++;

        if (condition.subjCond.N == 2)
        {
            removeSubj(SCREW_MAN);
            if (subjNum == amount)
            {
                msgsnd(subjCond, &condition, msgSize, 0);
                printf("screw man; ended my work for today\n");
                return 0;
            }
            needNew.mtype = subjNum + 1;
            needNew.needNew = 1;

            msgsnd(E, &needNew, msgSize, 0);

            long tmp = subjNum + 1;
            msgsnd(N, &tmp, 0, 0);
            msgsnd(N, &tmp, 0, 0);
        }
        else
        {
            printf("screw man; ended my work on %d subject\n", subjNum);
            msgsnd(subjCond, &condition, msgSize, 0);
            if (subjNum == amount)
            {
                printf("screw man; ended my work for today\n");
                return 0;
            }
            long tmp;
            msgrcv(S, &tmp, 0, 0, 0);
        }
    }

    return 0;
}

int main(int argc, char* argv[])
{
    int subjCond = msgget(IPC_PRIVATE, 0777); // msg queue, where condition of subject is sent and received
    int E = msgget(IPC_PRIVATE, 0777); // msg queue, for cases when new subject should be placed
    int S = msgget(IPC_PRIVATE, 0777); // msg queue for screw man to sleep when his work on current subject is done
    int B = msgget(IPC_PRIVATE, 0777); // msg queue for nut man to sleep when his work on current subject is done


    struct msgbuf init = {1, 1, 0, 0};
    msgsnd(E, &init, sizeof(init) - sizeof(long), 0);

    int N;
    if (argc > 1)
        N = atoi(argv[1]);
    else
        N = 5;

    pid_t childPid;
    for (int i = 0; i < 3; i++)
    {
        childPid = fork();

        if (childPid == 0)
        {
            if (i < 2)
            {
                printf("Calling screw man\n");
                nutMan(i + 1, N, E, subjCond, S, B);
                return 0;
            }
            else
            {
                printf("Calling screw man\n");
                screwMan(N, E, subjCond, S, B);
                return 0;
            }
        }
    }

    for (int i = 0; i < 3; i++)
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