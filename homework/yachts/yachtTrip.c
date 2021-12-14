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
#include <sys/time.h>


enum SEMAPHORES
{
    THE_TRAP = 0,
    THE_YACHT,
    THE_BANK,
    TO_GET_ONTO,
    TO_LEAVE,
    TO_PARTICIPATE,
    LAST_TRIP_WARNING,
    TRIPPING
};

//const struct sembuf operations [][2] = { getOnYacht, freeTrap, leaveYacht };

/* Task:

    There are NP passengers and 1 capitan;

    There is a yacht and it can hold NB people

    NT = 1 - ограничение перехода по трапу

    Нужно организовать посадку высадку и покатание

    Окончание покатаний - закат

    При прибытии на берег: все выходят и обратно садятся кто хочет, а точнее кто успеет

*/

/*

    given variables: NP, NB, NE; NE - param, which tells when this stops

*/

int Captain(int semid, int passengersCount, int yachtCap, int tripNum, int trapCap)
{
    printf("Hey, I am the captain, starting the voyage\n");

    int whatToWait = (passengersCount < yachtCap) ? THE_BANK : THE_YACHT;
    int maxCrowd   = (passengersCount < yachtCap) ? passengersCount : yachtCap;

    printf("waiting for the %d\n", whatToWait);

    struct sembuf waitFull [] = {{whatToWait, 0, 0}, {TO_GET_ONTO, 0, 0}};
    struct sembuf waitEmpty [] = {{TO_LEAVE, 0, 0}};
    struct sembuf arriveAtBeach [] = {{TO_LEAVE, maxCrowd, 0}};
    struct sembuf reGet [] = {{TO_GET_ONTO, maxCrowd, 0}};
    struct sembuf allowToParticipate [] = {TO_PARTICIPATE, passengersCount, 0};
    struct sembuf declareEnding [] = {LAST_TRIP_WARNING, 1, 0};
    struct sembuf tripStart [] = {TRIPPING, 1, 0};
    struct sembuf tripEnd [] = {TRIPPING, -1, 0};

    for (int i = 0; i < tripNum; i++)
    {
        printf("Hey, I am the captain, waiting for people to get on board\n");
        semop(semid, allowToParticipate, 1);
        semop(semid, reGet, 1);
        semop(semid, waitFull, 1);
        printf("Hey, I am the captain, my yacht is full / all the people are on board\n");
        printf("Starting trip\n");
        semop(semid, tripStart, 1);

        usleep(500000);

        printf("Ending trip\n");
        semop(semid, tripEnd, 1);

        printf("Hey, I am the captain, did a lap %D\n", i + 1);
        semop(semid, arriveAtBeach, 1);
        semop(semid, waitEmpty, 1);
        printf("Hey, I am the captain, my yacht is empty\n");
        if (i == tripNum - 1)
        {
            semop(semid, declareEnding, 1);
        }
    }

    printf("Hey, I am the captain, The trip is over!!\n");

    return 0;
}

int Passenger(int semid, pid_t myid, int tripNum, int trapCap)
{
    struct sembuf askForPermission[] = {{TO_PARTICIPATE, -1, IPC_NOWAIT}};
    struct sembuf getOnYacht [] = {{THE_TRAP, -1, IPC_NOWAIT}, {THE_YACHT, -1, IPC_NOWAIT}, {THE_BANK, -1, IPC_NOWAIT}, {TO_GET_ONTO, -1, IPC_NOWAIT}};
    struct sembuf freeTrap []   = {THE_TRAP, 1, 0};
    struct sembuf leaveYacht [] = {{THE_TRAP, -1, 0}, {THE_YACHT, 1, 0}, {THE_BANK, 1, 0}, {TO_LEAVE, -1, 0}};

    printf("Hey, I am a passenger %d\n", myid);

    //usleep(500000);

    while(1)
    {
        int flag = 0;

        while ((semop(semid, askForPermission, 1)) < 0 && (errno == EAGAIN))
        {
            if (semctl(semid, LAST_TRIP_WARNING, GETVAL))
            {
                return 0;
            }
        }

        while ((semop(semid, getOnYacht, 4) < 0) && (errno == EAGAIN))
            if (semctl(semid, TRIPPING, GETVAL))
            {
                flag = 1;
                break;
            }
        
        if (flag)
            continue;

        printf("Hey, I am passenger %d, got onto yacht\n", myid);
        semop(semid, freeTrap, 1);
        printf("Hey, I am passenger %d, freed the trap\n", myid);

        semop(semid, leaveYacht, 4);
        printf("Hey, I am passenger %d, leaving the yacht\n", myid);
        //sleep(2);
        semop(semid, freeTrap, 1);
        printf("Hey, I am passenger %d, freed the trap\n", myid);
    }

    //semop

    return 0;
}


int main(int argc, char* argv[])
{
    if (argc < 4)
    {
        printf("Not enough arguments\n");
        return 0;
    }

    int passengersCount = atoi(argv[1]);
    int yachtCapacity   = atoi(argv[2]);
    int tripsNum        = atoi(argv[3]);
    int trapCap;
    if (argc >= 5)
        trapCap = atoi(argv[4]);
    else
        trapCap = 1;
    printf("trapCap - %d\n", trapCap);


    int semid = semget(IPC_PRIVATE, 8, 0777);
    if (semid < 0)
    {
        perror("There apperead an error while trying to initialise a semaphore:");
        exit(1);
    }

    struct sembuf initialiseSems [] = {{THE_TRAP, trapCap, 0}, {THE_YACHT, yachtCapacity, 0}, {THE_BANK, passengersCount, 0}};
    semop(semid, initialiseSems, 3);

    pid_t captain = fork();

    if (captain == 0)
    {
        Captain(semid, passengersCount, yachtCapacity, tripsNum, trapCap);
        return 0;
    }

    pid_t passengerPid;
    for (int i = 0; i < passengersCount; i++)
    {
        passengerPid = fork();
        if (passengerPid < 0)
            printf("Something gone wrong when forking for a runner %d: %s", i, strerror(errno));
        
        if (passengerPid == 0)
        {
            Passenger(semid, i, tripsNum, trapCap);
            return 0;
        }
    }


    for (int i = 0; i < passengersCount + 1; i++)
    {
        int status;
        wait(&status);
        printf("\nmain proc waited %d\n", i);
    }

    semctl(semid, 0, IPC_RMID);

}