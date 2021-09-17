#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pwd.h>
#include <uuid/uuid.h>
#include <grp.h>
#include <sys/param.h>

#define MAX_GROUPS 32

typedef struct Pros // creating a structure that will hold all needed info
{
    struct passwd* userinfo;
    struct group* groupinfo;
    gid_t* grouplist;
    int nGroups;
}Process;

Process* getCurrent(Process* Calling);
Process* getByName(Process* Calling, char* name);
void print(Process* Calling);

int main(int argc, char* argv[])
{
    Process Calling;
    Calling.nGroups = MAX_GROUPS;

    switch (argc)
    {
    case 1: getCurrent(&Calling);
            break;
    case 2: getByName(&Calling, argv[1]);
            break;
    default: printf("You were supposed to give just the name of the process");
             return 0;
    }
    print(&Calling);


    return 0;
}

Process* getCurrent(Process* Calling)
{
    uid_t userID = getuid();
    uid_t groupID = getgid();
    Calling->grouplist = (gid_t*)calloc(Calling->nGroups, sizeof(gid_t*));

    Calling->userinfo = getpwuid(userID);
    Calling->groupinfo = getgrgid(groupID);
    Calling->nGroups = getgroups (Calling->nGroups, Calling->grouplist);
    
    return Calling;
}

Process* getByName(Process* Calling, char* name)
{
    Calling->grouplist = (gid_t*)calloc(Calling->nGroups, sizeof(gid_t));
    Calling->userinfo = getpwnam(name);

    if (Calling->userinfo == 0)
    {
        printf("myid: %s: no such user\n", name);
        exit(1);
    }

    Calling->groupinfo = getgrgid(Calling->userinfo->pw_gid);
    int flag = getgrouplist (Calling->userinfo->pw_name, Calling->groupinfo->gr_gid, (int*)Calling->grouplist, &(Calling->nGroups) );

    if (flag)
    {
        printf("Something gone wrong when trying to get grouplist, maybe the process has number "
               "of suplementary groups exceeding NGROUPS_MAX (defined in sys/limits.h)\n");
        exit(1);
    }

    return Calling;
}

void print(Process* Calling)
{
    printf("uid=%llu(%s)", (long long unsigned)Calling->userinfo->pw_uid, Calling->userinfo->pw_name);
    printf(" gid=%llu(%s) ", (long long unsigned)Calling->groupinfo->gr_gid, (Calling->groupinfo->gr_name));

    printf("groups=");
    for (int i = 0; i < Calling->nGroups; i++)
    {
        Calling->groupinfo = getgrgid(Calling->grouplist[i]);

        printf("%d(%s)", Calling->grouplist[i], (Calling->groupinfo->gr_name));
        if (i < (Calling->nGroups - 1))
            printf(",");
    }

    printf("\n");

    free(Calling->grouplist); 
}