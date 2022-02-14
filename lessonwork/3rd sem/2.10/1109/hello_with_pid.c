#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>

int main()
{
    printf("Hello world!\n");

    pid_t pidOfProg = getpid();
    pid_t pidOfParent = getppid(); // this pid does not change
                                   // because it is bash
    
    printf("my pid = %lld; my parent pid = %lld\n", (long long)pidOfProg, (long long)pidOfParent);
}
