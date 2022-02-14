#include <stdio.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <stdlib.h>
#include <unistd.h>

#define FULL_ACCESS 0777

enum semaphors
{
	STAIR_DOWN = 0,
	NUM_ON_BOAT,
	NUM_ON_STAIR,
	NUM_ON_COAST,
	START_OF_TRIP,
	END_OF_TRIP,
	LAST_TRIP
};

const int sems_num = 7;

#define TRY(FUNC)			\
	if(FUNC < 0)			\
	{				\
		perror("Error" #FUNC);	\
		exit(-1);		\
	}				\

void V(int sem_id, enum semaphors sem_num, int num)
{
	struct sembuf sem_op = {sem_num, num, 0};
	TRY(semop(sem_id, &sem_op, 1));
}

void P(int sem_id, enum semaphors sem_num, int num)
{
	num = -num;
	struct sembuf sem_op = {sem_num, num, 0};
	TRY(semop(sem_id, &sem_op, 1));
}

void Z(int sem_id, enum semaphors sem_num)
{
	struct sembuf sem_op = {sem_num, 0, 0};
	TRY(semop(sem_id, &sem_op, 1));
}

void Passenger(int sem_id, int ind)
{
	int ride = 0;

	while(1)
	{
		Z(sem_id, STAIR_DOWN);

		if(semctl(sem_id, LAST_TRIP, GETVAL))
		{
			break;
		}

		printf("Passenger number %d is ready\n", ind);

		P(sem_id, NUM_ON_BOAT, 1);

		if(semctl(sem_id, LAST_TRIP, GETVAL))
		{
			break;
		}
	
		P(sem_id, NUM_ON_STAIR, 1);
		printf("Passenger number %d stepped on the stair\n", ind);
		sleep(1);
		V(sem_id, NUM_ON_STAIR, 1);
		printf("Passenger number %d on the boat\n", ind);

		Z(sem_id, START_OF_TRIP);
		Z(sem_id, END_OF_TRIP);

		Z(sem_id, STAIR_DOWN);
		P(sem_id, NUM_ON_STAIR, 1);
		printf("Passenger number %d is going down\n", ind);
		V(sem_id, NUM_ON_STAIR, 1);
		sleep(1);
		printf("Passenger number %d has gone down\n", ind);
		V(sem_id, NUM_ON_BOAT, 1);
		P(sem_id, NUM_ON_COAST, 1);
		ride = 1;
	}
	if(ride)
	{
		printf("\033[0;32m");
		printf("###Passenger %d was on the trip###\n", ind);
		printf("\033[0m");
	}
	else
	{
		printf("\033[0;31m");
		printf("///Passenger %d was not on the trip///\n", ind);
		printf("\033[0m");
	}
}

void Captain(int sem_id, int num_trips, int size_stair, int size_boat)
{
	P(sem_id, STAIR_DOWN, 1);
	P(sem_id, START_OF_TRIP, 1);

	for(int i = 0; i < num_trips; ++i)
	{
		if(i == num_trips - 1)
		{
			V(sem_id, LAST_TRIP, 1);
		}

		printf("Stair was downed, go on the boat\n");

		Z(sem_id, NUM_ON_BOAT);

		V(sem_id, STAIR_DOWN, 1);
		
		P(sem_id, NUM_ON_STAIR, size_stair);
		V(sem_id, NUM_ON_STAIR, size_stair);

		printf("###START###\n");
		V(sem_id, START_OF_TRIP, 1);
		sleep(5);
		P(sem_id, END_OF_TRIP, 1);
		printf("###END###\n");

		P(sem_id, STAIR_DOWN, 1);
		Z(sem_id, NUM_ON_COAST);
		printf("Another group!\n");
		V(sem_id, END_OF_TRIP, 1);
		V(sem_id, NUM_ON_COAST, size_boat);

		if(i == num_trips - 1)
		{
			break;
		}

		P(sem_id, START_OF_TRIP, 1);
	}
	printf("Captain goes home\n");
}

int main(int argc, char** argv)
{
	int num_pass = 10;
	int num_trips = 2;
	int size_boat = 2;
	int size_stair = 1;

	if(argc >= 5)
	{
		num_pass = atoi(argv[1]);
		num_trips = atoi(argv[2]);
		size_boat = atoi(argv[3]);
		size_stair = atoi(argv[4]);
	}

	int sem_id = semget(IPC_PRIVATE, sems_num, FULL_ACCESS);
	TRY(sem_id);

	V(sem_id, STAIR_DOWN, 1);
	V(sem_id, END_OF_TRIP, 1);
	V(sem_id, START_OF_TRIP, 1);
	V(sem_id, NUM_ON_STAIR, size_stair);

	if(size_boat < num_pass)
	{
		V(sem_id, NUM_ON_BOAT, size_boat);
		V(sem_id, NUM_ON_COAST, size_boat);
	}
	else
	{
		V(sem_id, NUM_ON_BOAT, num_pass);
		V(sem_id, NUM_ON_COAST, num_pass);
	}

	pid_t captain, pass;
	
	captain = fork();
	TRY(captain);
	
	if(captain == 0)
	{
		if(size_boat < num_pass)
		{
			Captain(sem_id, num_trips, size_stair, size_boat);
		}
		else
		{
			Captain(sem_id, num_trips, size_stair, num_pass);
		}
		return 0;
	}

	for(int i = 0; i < num_pass; i++)
	{
		pass = fork();
		TRY(pass);

		if(pass == 0)
		{
			Passenger(sem_id, i + 1);
			return 0;
		}
	}

	return 0;
}