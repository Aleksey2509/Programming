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
#include <sys/stat.h>
#include <sys/param.h>
#include <getopt.h>
#include <dirent.h>
#include <fcntl.h>
#include <time.h>

#define BUFFSIZ 4096

const char flags [] = "lainr";

struct option longopts[] = 
{
    { "long",          no_argument,      NULL,     'l' },
    { "all",           no_argument,      NULL,     'a' },
    { "inode",         no_argument,      NULL,     'i' },
    { "numeric",       no_argument,      NULL,     'n' },
    { "recoursive",    no_argument,      NULL,     'r' }
};

typedef struct FlagStruct
{
    int lflag;
    int aflag;
    int iflag;
    int nflag;
    int rflag;
}Flags;

Flags FlagState = {0};

int myls (char* dirPath, DIR* newdir, int argcount);
int parse(int argcount, char* arguments[]);
int printFileInfo (const struct stat* statFile);


int getFilePath (const char* dirPath, char* filePath, const char* fileName)
{
    //printf("dirpath - %s, filename - %s\n", dirPath, fileName);
    fflush(stdout);
    filePath = strcpy(filePath, dirPath);
    int dirLen = strlen(dirPath);

    if (dirPath[dirLen - 1] != '/')
    {
        filePath[dirLen + 1] = '\0';
        filePath[dirLen] = '/';
    }
    filePath = strcat(filePath, fileName);

    return 0;
}

int parse(int argcount, char* arguments[])
{
    DIR* newDir;
    for (int i = 0; i < argcount; i++)
    {
        if ((newDir = opendir(arguments[i])) != 0)
        {
            printf("%s:\n", arguments[i]);
            myls(arguments[i], newDir, argcount);
        }
    }
    return 0;
}

char* getAccessInfo (char* access, mode_t mode)
{
    for (int i = 0; i < 10; i++)
        access[i] = '-';

    if (mode & S_IFDIR)
        access [0] = 'd';

    if (mode & S_IRUSR)
        access [1] = 'r';
    
    if (mode & S_IWUSR)
        access [2] = 'w';
    
    if (mode & S_IXUSR)
        access [3] = 'x';
    
    if (mode & S_IRGRP)
        access [4] = 'r';
    
    if (mode & S_IWGRP)
        access [5] = 'w';
    
    if (mode & S_IXGRP)
        access [6] = 'x';
    
    if (mode & S_IROTH)
        access [7] = 'r';

    if (mode & S_IWOTH)
        access [8] = 'w';

    if (mode & S_IXOTH)
        access [9] = 'x';

    return access;
}

int myls (char* dirPath, DIR* newdir, int argcount)
{
    struct dirent* newEntry;
    struct stat statFile;
    char* filePath = (char*)calloc(BUFSIZ, sizeof(char));



    while( (newEntry = readdir(newdir)) != NULL)
    {
        if (!FlagState.aflag)
        {
            if (newEntry->d_name[0] == '.')
                continue;
        }

        getFilePath(dirPath, filePath, newEntry->d_name);
        //printf("filePath = %s\n", filePath);

        if (lstat (filePath, &statFile) != 0)
        {
            printf("Some error occured when getting stat of %s: %s\n", filePath, strerror(errno));
            continue;
        }

        if (FlagState.iflag)
            printf("%10llu ", statFile.st_ino);

        if (FlagState.lflag || FlagState.nflag)
            printFileInfo(&statFile);

        printf("%s\n", newEntry->d_name);

        memset(filePath, 0, BUFFSIZ);
    }

    free(filePath);

    return 0;
}

int printFileInfo (const struct stat* statFile)
{

    char* time = ctime(&(statFile->st_mtimespec.tv_sec));
    time[strlen(time) - 1] = '\0';

    char* access = (char*)calloc(12, sizeof(char));
    getAccessInfo(access, statFile->st_mode);

    if (FlagState.nflag)
        printf("%s %2hu %u %u %10llu %s %llu ", access, statFile->st_nlink, statFile->st_uid, statFile->st_gid, statFile->st_size, time, statFile->st_blocks);
    else
    {
        struct passwd* userinfo = getpwuid(statFile->st_uid);
        struct group* groupinfo = getgrgid(statFile->st_gid);
        printf("%s %2hu %s %s %10llu %s %llu ", access, statFile->st_nlink, userinfo->pw_name, groupinfo->gr_name, statFile->st_size, time, statFile->st_blocks);
    }

    free(access);
    return 0;
}

int main(int argc, char* argv[])
{
    int newFlag = 0;

    while ((newFlag = getopt_long (argc, argv, flags, longopts, NULL)) != -1)
    switch (newFlag)
    {
        case 'l':
            FlagState.lflag = 1;
            break;
        case 'a':
            FlagState.aflag = 1;
            break;
        case 'i':
            FlagState.iflag = 1;
            break;
        case 'n':
            FlagState.nflag = 1;
            break;
        case 'r':
            FlagState.rflag = 1;
            break;
    }

    if (argc - optind < 1)
    {
        DIR* newdir = opendir("./");
        myls("./", newdir, 1);
    }
    else
        parse(argc - optind, argv + optind);

    return 0;
}