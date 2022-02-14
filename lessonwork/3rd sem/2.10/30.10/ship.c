#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <stdlib.h>

#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/wait.h>
#include <sys/types.h>

#define MAX_MEM_SIZE  4096
#define ACCESS        0777
#define SEM_NUM          7

#define ERR_CHECK( statement ) \
  if (statement)               \
  {                            \
    perror(#statement);        \
    _exit(1);                  \
  }                            \

typedef struct sembuf sembuf;

typedef enum SemInd
{
  SUN = 0,
  TRIP,
  SHIP,
  READY,
  LADDER,
  DOCK_IN,
  DOCK_OUT
} SemInd;

int PASS_NUM   = 0,
    SHIP_CAP   = 0,
    DOCK_CAP   = 0,
    LADDER_CAP = 0;

void P( int sem_id, SemInd ind, short delta );
void V( int sem_id, SemInd ind, short delta );
void Z( int sem_id, SemInd ind );
int Z_NO_WAIT( int sem_id, SemInd ind );


int process_cap( int sem_id, int rides );
int process_pass( int sem_id, int num );



int main( int argc, char ** argv )
{
  /* Arguments checking */
  if (argc < 5)
  {
    printf("USAGE: ./chill PASSENGERS_NUM "
           "LADDER_CAP "
           "SHIP_CAP "
           "RIDE_NUM\n");
    return 0;
  }

  /* Buffer setting */
  char print_buf[MAX_MEM_SIZE];
  setvbuf(stdout, print_buf, _IOLBF, MAX_MEM_SIZE);

  /* Vars init and other stuff */
  PASS_NUM     = atoi(argv[1]);
  LADDER_CAP   = atoi(argv[2]);
  SHIP_CAP     = atoi(argv[3]);
  int ride_num = atoi(argv[4]);

  SHIP_CAP     = SHIP_CAP < PASS_NUM ? SHIP_CAP : PASS_NUM;
  DOCK_CAP     = SHIP_CAP;

  int sem_id = semget(IPC_PRIVATE, SEM_NUM, ACCESS);

  /* Capitan fork and processing */

  //V(sem_id, SUN, 1);
  V(sem_id, TRIP, 1);

  pid_t cap_pid = fork();

  if (cap_pid == 0)
    return process_cap(sem_id, ride_num);
  else
    ERR_CHECK(cap_pid < 0);

  /* Passenger fork and processing */
  for (int i = 0; i < PASS_NUM; ++i)
  {
    pid_t pass_pid = fork();

    if (pass_pid == 0)
    {
      int total_chill = process_pass(sem_id, i);
      printf("#%d chilled total: %d\n", i, total_chill);
      return 0;
    }
    else
      ERR_CHECK(pass_pid < 0);
  }

  for (int i = 0; i < PASS_NUM + 1; ++i)
      ERR_CHECK(wait(NULL) < 0);

  /* finishing */
  semctl(sem_id, 0, IPC_RMID);
  return 0;
}

int process_cap( int sem_id, int rides )
{
    printf("LADDER OPENING\n");
  V(sem_id, LADDER, LADDER_CAP);

    printf("SHIP OPENING\n");
  V(sem_id, SHIP, SHIP_CAP);

  for (int i = 0; i < rides; ++i)
  {
      printf("DOCK ENTER OPENING\n");
    V(sem_id, DOCK_IN, DOCK_CAP);

    Z(sem_id, DOCK_IN);
      printf("MAXIMUM PEOPLE ENTERED DOCK\n");

    Z(sem_id, DOCK_OUT);
      printf("MAXIMUM PEOPLE LEFT DOCK\n");

    P(sem_id, READY, DOCK_CAP);
      printf("EVERYBODY READY\n");

    P(sem_id, LADDER, LADDER_CAP);
      printf("LADDER CLOSED\n");

    P(sem_id, TRIP, 1);
      printf("------------------TRIPPING-----------------\n");
    V(sem_id, TRIP, 1);

      printf("OPENING DOCK OUT\n");
    V(sem_id, DOCK_OUT, DOCK_CAP);

      printf("OPENING LADDER\n");
    V(sem_id, LADDER, LADDER_CAP);
  }

  V(sem_id, SUN, 1);

  return 0;
}

int process_pass( int sem_id, int num )
{
  int count = 0;

  while (1)
  {
    if (Z_NO_WAIT(sem_id, SUN) == -1 && errno ==  EAGAIN)
    {
      printf("    #%03d leave\n", num);
      break;
    }

    P(sem_id, DOCK_IN, 1);
      printf("    #%03d entered DOCK\n", num);

    P(sem_id, LADDER, 1);
      printf("    #%03d entered LADDER\n", num);

      printf("    #%03d leaving LADDER\n", num);
    V(sem_id, LADDER, 1);

    P(sem_id, SHIP, 1);
      printf("    #%03d entered SHIP\n", num);

      printf("    #%03d ready to trip\n", num);
    V(sem_id, READY, 1);

      printf("    #%03d waiting trip\n", num);
    Z(sem_id, TRIP);
      printf("    #%03d TRIPPING\n", num);
      ++count;

      printf("    #%03d leaving SHIP\n", num);
    V(sem_id, SHIP, 1);

    P(sem_id, LADDER, 1);
      printf("    #%03d entered LADDER\n", num);

      printf("    #%03d left LADDER\n", num);
    V(sem_id, LADDER, 1);

    P(sem_id, DOCK_OUT, 1);
      printf("    #%03d left DOCK\n", num);

    if (Z_NO_WAIT(sem_id, SUN) == -1 && errno ==  EAGAIN)
    {
      printf("    #%03d leave\n", num);
      break;
    }
  }

  return count;
}

void P( int sem_id, SemInd ind, short delta )
{
  sembuf buf = {ind, -delta, 0};
  ERR_CHECK(semop(sem_id, &buf, 1) > 0);
}

void V( int sem_id, SemInd ind, short delta )
{
  sembuf buf = {ind, delta, 0};
  ERR_CHECK(semop(sem_id, &buf, 1) > 0);
}

void Z( int sem_id, SemInd ind )
{
  sembuf buf = {ind, 0, 0};
  ERR_CHECK(semop(sem_id, &buf, 1) > 0);
}

int Z_NO_WAIT( int sem_id, SemInd ind )
{
  sembuf buf = {ind, 0, IPC_NOWAIT};
  return semop(sem_id, &buf, 1);
}
