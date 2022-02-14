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

#define BUFSIZE 4096

typedef struct msgbuf
{
    long mtype;
    char* mtext;
}Message;

int printTime (struct timeval tp1, struct timeval tp2)
{
    long sec = tp2.tv_sec - tp1.tv_sec;
    long microsec = tp2.tv_usec - tp1.tv_usec;
    double TimePassed = (double)sec + ( (double)microsec ) / 1000000;

    printf("%lf\n\n", TimePassed);
    return 0;
}

int mysend(int msqid, void* msgp, size_t msgsz, int msgflg)
{
    int Error = msgsnd(msqid, msgp, msgsz, msgflg);
    if (Error < 0)
    {
        printf("Something gone wrong when sending a message: %s\n",  strerror(errno));
        exit(1);
    }
    return 0;
}

int myreceive(int msqid, void* msgp, size_t msgsz, long msgtype, int msgflg)
{
    int Error = msgrcv(msqid, msgp, msgsz, msgtype, msgflg);
    if (Error < 0)
    {
        printf("Something gone wrong when when receiving a message: %s\n", strerror(errno));
        exit(1);
    }
    return 0;
}

int judge (int N, int msgId)
{
    printf("Hey, I am a judge\n");
    char* buffer[BUFSIZE] = { 0 };
    long arrived = 1;
    for (int i = 0; i < N; i++)
    {
        myreceive(msgId, buffer, 0, arrived, 0);
        //printf("Got arrived an runner number %d\n", i);
    }
    printf("Judge: Hey everyone! This is your competition! At the shot of the barrel the first runner can start!\n\n");

    long StartShot = 2;
    struct timeval tp1;
    gettimeofday(&tp1, NULL);
    mysend(msgId, &StartShot, 0, 0);

    long finishFlag = N + 2;
    myreceive(msgId, buffer, 0, finishFlag, 0);

    struct timeval tp2;
    gettimeofday(&tp2, NULL);

    printf("Hey, this is judge, the competition is finished! The result is:");
    printTime (tp1, tp2);

    long int compStop = N + 3;
    for (int i = 0; i < N; i++)
        mysend(msgId, &compStop, 0, 0);

    return 0;
}

int runner(int N, int msgId, long int runnerId)
{
    printf("Hey, I am a runner with id %ld \n", runnerId);

    long arrived = 1;
    mysend(msgId, &arrived, 0, 0);

    long myStartFlag = runnerId + 2;
    //printf("I am runner %ld, my start flag - %ld\n\n", runnerId, myStartFlag);
    char* buffer[BUFSIZE] = { 0 };
    myreceive(msgId, buffer, 0, myStartFlag, 0);

    printf("I am runner %ld; I started running\n", runnerId);
    printf("I am runner %ld; I stopped running\n\n", runnerId);

    long giveToNext = myStartFlag + 1;
    mysend(msgId, &giveToNext, 0, 0);

    long int compStop = N + 3;
    myreceive(msgId, buffer, 0, compStop, 0);
    printf("Thank you, i am runner %ld, i received message about ending\n", runnerId);

    return 0;
}

int relayRace(int argc, char* argv[])
{
    int id = msgget(IPC_PRIVATE, 0777);

    if (id < 0)
        printf("Something gone wrong when creating a message: %s", strerror(errno));

    int N = atoi(argv[1]);

    pid_t judgePid = fork();
    if (judgePid < 0)
        printf("Something gone wrong when forking for a judge: %s", strerror(errno));

    if (judgePid == 0)
    {
        judge(N, id);
        return 0;
    }

    pid_t runnersPid;
    for (int i = 0; i < N; i++)
    {
        runnersPid = fork();
        if (runnersPid < 0)
            printf("Something gone wrong when forking for a runner %d: %s", i, strerror(errno));
        
        if (runnersPid == 0)
        {
            runner(N, id, i);
            return 0;
        }
    }

    for (int i = 0; i < N + 1; i++)
    {
        int status;
        wait(&status);
        //printf("\nwaited %d\n", i);
    }
    msgctl(id, IPC_RMID, 0);


    return 0;
}

int main(int argc, char* argv[])
{
    if (argc < 2)
    {
        printf("Give number of runners");
        return 0;
    }

    relayRace(argc, argv);

    return 0;
}