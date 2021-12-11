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

int removeSubj(int workerId, int E, int subjNum)
{
    switch (workerId)
    {
        case SCREW_MAN: printf("Hey, i am a screw man, i am removing subject\n");
                        break;
        case BOLT_MAN: printf("Hey, i am a bolt man, i am removing subject\n");
                        break;
    }

    struct msgbuf needNew = {subjNum, 1, 0, 0};
    long msgSize = sizeof(struct msgbuf) - sizeof(long);

    msgsnd(E, &needNew, msgSize, 0);

    return 0;
}

int placeNew(int workerId, int E, int subjCondQueue, int subjNum)
{
    switch (workerId)
    {
        case SCREW_MAN: printf("Hey, i am a screw man, i am placing new subject\n");
                        break;
        case BOLT_MAN: printf("Hey, i am a bolt man, i am placing new subject\n");
                        break;
    }

    struct msgbuf condition = {subjNum, 0, 0, 0};
    struct msgbuf needNew = {subjNum, 0, 0, 0};
    long msgSize = sizeof(struct msgbuf) - sizeof(long);

    msgsnd(E, &needNew, msgSize, 0);
    msgsnd(E, &needNew, msgSize, 0);
    msgsnd(subjCondQueue, &condition, msgSize, 0);

    return 0;
}

int screwMan(int N, int E, int subjCond)
{
    printf("Hey, i am a screw man\n");
    struct msgbuf condition = {1, 0, 0, 0};
    struct msgbuf needNew;
    int msgSize = sizeof(struct msgbuf) - sizeof(long);

    for (int i = 0; i < N; i++)
    {
        printf("screw man, working on %d subj\n", i + 1);

        msgrcv(E, &needNew, msgSize, i + 1, 0);

        if (needNew.needNew == 1)
        {
            placeNew(SCREW_MAN, E, subjCond, i + 1);
        }

        msgrcv(subjCond, &condition, msgSize, i + 1, 0);
        placeScrew();
        condition.subjCond.S++;

        if ( (condition.subjCond.S == 2) && (condition.subjCond.B == 1) )
        {
            removeSubj(SCREW_MAN, E, i + 2);
        }
        else
        {
            //printf("screw man, sent with type %ld\n", condition.mtype);
            msgsnd(subjCond, &condition, msgSize, 0);
        }
    }

    return 0;
}

int boltMan(int N, int E, int subjCond)
{
    printf("Hey, i am a bolt man\n");
    struct msgbuf condition = {1, 0, 0, 0};
    struct msgbuf needNew;
    int msgSize = sizeof(struct msgbuf) - sizeof(long);
    //printf("msgSize - %d\n", msgSize);

    for (int i = 0; i < N; i++)
    {
        printf("bolt man, working on %d subj\n", i + 1);
        msgrcv(E, &needNew, msgSize, i + 1, 0);
        //printmsg(BOLT_MAN, needNew);
        if (needNew.needNew == 1)
        {
            placeNew(BOLT_MAN, E, subjCond, i + 1);
        }

        msgrcv(subjCond, &condition, msgSize, i + 1, 0);
        placeBolt();
        condition.subjCond.B++;

        if ( (condition.subjCond.S == 2) && (condition.subjCond.B == 1) )
        {
            removeSubj(BOLT_MAN, E, i + 2);
        }
        else
        {
            //printf("bolt man, sent with type %ld\n", condition.mtype);
            msgsnd(subjCond, &condition, msgSize, 0);
        }
    }

    return 0;
}

int main(int argc, char* argv[])
{
    int E = msgget(IPC_PRIVATE, 0777);
    int subjCond = msgget(IPC_PRIVATE, 0777);
    //printf("E - %d, subjcond - %d\n", E, subjCond);
    int N;
    if (argc > 1)
        N = atoi(argv[1]);
    else
        N = 3;

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
                screwMan(N, E, subjCond);
                return 0;
            }
            else
            {
                printf("Calling bolt man\n");
                boltMan(N, E, subjCond);
                return 0;
            }
        }
    }

    printf("main sending a init message\n");
    struct msgbuf init = {1, 1, 0, 0};
    //printmsg(0, init);
    msgsnd(E, &init, sizeof(init) - sizeof(long), 0);

    for (i = 0; i < 3; i++)
    {
        int status;
        wait(&status);
    }

    printf("main waited for children and freeing msg queues\n");

    msgctl(E, IPC_RMID, 0);
    msgctl(subjCond, IPC_RMID, 0);
}