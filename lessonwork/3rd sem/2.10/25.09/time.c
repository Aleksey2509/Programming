#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <unistd.h>

int printtime (struct timeval tp1, struct timeval tp2)
{
    long long microsec = tp2.tv_usec - tp1.tv_usec;
    long long sec = tp2.tv_sec - tp1.tv_sec;

    sec += microsec / 1000000;

    printf("%lld.%06lld", sec, microsec);
    return 0;
}

int main(int argc, char* argv[])
{
    struct timeval tp1; 
    struct timeval tp2;

    gettimeofday(&tp1, NULL);

    usleep(10000);
    gettimeofday(&tp2, NULL);

    printtime(tp1, tp2);
}