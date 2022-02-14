#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <ctype.h>
#include <unistd.h>
#include <uuid/uuid.h>
#include <errno.h>
#include <sys/stat.h>
#include <sys/param.h>
#include <getopt.h>
#include <dirent.h>
#include <fcntl.h>

const int READ = 0x0000;
const int WRITE = 0x0001;

const char flags [] = "fiv";

struct option longopts[] = 
{
    { "verbose",       no_argument,      NULL,     'v' },
    { "interactive",   no_argument,      NULL,     'i' },
    { "force",         no_argument,      NULL,     'f' },
};

enum CODES
{
    FAILURE = -100,
    CANT_ALLOC_MEM,
    STOPPED,
    NON_EXIST,
    EXISTS = 1
};

typedef struct FlagStruct
{
    int fflag;
    int vflag;
    int iflag;
}Flags;

int interactive(char* fileName);
int force(char* fileName);
int mywrite (int fd, char* buffer, int size);
int myread (int fd, char* buffer, int size);
int fileReadWrite(char* fileFrom, char* fileTo, Flags flagState);
char* getNewFilePath (char* dirpath, size_t dirLen, char* oldFilePath, int pathLen, char* newFilePath, int currentNameSize);
char* reallocateName (char* name, int length, int* currentNameSize);
int workWithDir (char* fileArr[], int fileCount, Flags flagState);
int mycp(int argcount, char* arguments[]);

//------------------------------------------------------------------------------------------------------------------------

int mywrite (int fd, char* buffer, int size)
{
    int countWritten = 0;
    int leftToWrite = size;
    
    do
    {
        countWritten = write(fd, buffer + countWritten, size);
        if (countWritten < 0)
        {
            return FAILURE;
        }

        leftToWrite -= countWritten;
    }while ((countWritten != 0) && (leftToWrite != 0));

    return 0;
}

//------------------------------------------------------------------------------------------------------------------------

int myread (int fd, char* buffer, int size)
{
    int ReadSize = read(fd, buffer, BUFSIZ);
    if (ReadSize < 0)
    {
        return FAILURE;
    }

    return ReadSize;
}

//------------------------------------------------------------------------------------------------------------------------

int checkIfDir(char* path)
{
    printf("path to pssible dir - %s\n", path);
    struct stat statFile = { 0 };
    int error = stat (path, &statFile);
    printf("error - %d, can write - %d, is dir - %d, strerror - %s\n", error, statFile.st_mode & S_IWRITE, statFile.st_mode & S_IFDIR, strerror(errno));
    if ((error == 0) && ((statFile.st_mode & S_IWRITE) == 0))
        return -1;

    return statFile.st_mode & S_IFDIR;
}

//------------------------------------------------------------------------------------------------------------------------

int interactive(char* fileName)
{
    int fd = open(fileName, O_WRONLY | O_CREAT | O_EXCL);
    if (errno == EEXIST)
    {
        char userAnswer = 0; /* char for getting user answer */
        printf("overwrite %s? (y/n [n]) ", fileName);

        while ( (userAnswer = getchar()) == '\n') /* emptying the input buffer from previous '/n' */
        ;
        if ( tolower(userAnswer) != 'y' )
        {
            printf("not overwritten\n");
            return STOPPED;
        }

        fd = open(fileName, O_WRONLY | O_TRUNC);
    }

    if (fd < 0)
    {
        fprintf(stderr, "There appeared a problem with interactive opening file %s\n", fileName);
    }

    return fd;
}

//------------------------------------------------------------------------------------------------------------------------

int force(char* fileName)
{
    int fd = open(fileName, O_WRONLY);
    if (fd < 0)
    {
        remove(fileName);
        fd = open(fileName, O_WRONLY | O_CREAT);
    }

    if (fd < 0)
    {
        fprintf(stderr, "There appeared a problem with force opening file %s\n", fileName);
    }

    return fd;
}

//------------------------------------------------------------------------------------------------------------------------

int fileReadWrite(char* fileFrom, char* fileTo, Flags flagState)
{
    char buffer [BUFSIZ] = { 0 };

    int fdFrom, fdTo;

    if (flagState.iflag)
        fdTo = interactive(fileTo);

    if (flagState.fflag)
        fdTo = force(fileTo);

    if ((flagState.iflag == 0) && (flagState.fflag == 0))
        fdTo = open(fileTo, O_WRONLY | O_CREAT | O_TRUNC);

    fdFrom = open(fileFrom, O_RDONLY);

    if (fdFrom == FAILURE || fdTo == FAILURE || fdTo == STOPPED)
        goto error;

    while(1)
    {
        int ReadFlag = myread(fdFrom, buffer, BUFSIZ);
        if (ReadFlag == FAILURE)
        {
            fprintf(stderr, "There appeared an error with reading from %s: %s\n", fileFrom, strerror(errno));
            goto error;
        }

        int WriteFlag = mywrite(fdTo, buffer, ReadFlag);
        if (WriteFlag == FAILURE)
        {
            fprintf(stderr, "Appeared an error with writing to %s: %s\n", fileTo, strerror(errno));
            goto error;
        }

        if (ReadFlag == 0)
            break;
    }
    close (fdFrom);
    close (fdTo);

    if (flagState.vflag)
        {
            printf("%s -> %s\n", fileFrom, fileTo); /* verbose flag mode */
        }

    return 0;

    error:
        if (fdFrom > 0)
            close (fdFrom);
        if (fdTo > 0)
            close (fdTo);
        return FAILURE;
}

//------------------------------------------------------------------------------------------------------------------------

char* getNewFilePath (char* dirpath, size_t dirLen, char* oldFilePath, int pathLen, char* newFilePath, int currentNameSize)
{
    if (pathLen + dirLen > currentNameSize)
        {
            newFilePath = reallocateName(newFilePath, dirLen + pathLen + 1, &currentNameSize);
        }
    if (currentNameSize == 0)
        return oldFilePath;

    newFilePath = strcpy(newFilePath, dirpath); /* copy to newpath the path to directory where the file should be created */

    if ( dirpath[dirLen - 1] != '/' ) /* add a '/' after the directory name, if it is not there */
        {
            newFilePath[dirLen] = '/';
            newFilePath[dirLen + 1] = '\0';
        }

    int fileLen = 0;                                                /* get the lenth of the file name: start from the */
    for (int j = pathLen; (j >= 0) && (oldFilePath[j] != '/'); j--) /* end of path and count till a '/' is encountered or */
        fileLen++;                                                  /* the path has ended */

    newFilePath = strcat(newFilePath, (oldFilePath + pathLen - fileLen + 1)); /* get a new file path by catting the file name to the dir path */

    return newFilePath;
}

//------------------------------------------------------------------------------------------------------------------------

char* reallocateName (char* name, int length, int* currentNameSize)
{
    char* tmp = (char*)realloc(name, length);
    if (tmp == NULL)
    {
        *currentNameSize = 0;
        return name;
    }

    name = tmp; /* if path are too long - reallocate */
    *currentNameSize = length;

    return name;
}

//------------------------------------------------------------------------------------------------------------------------

int workWithDir (char* fileArr[], int fileCount, Flags flagState)
{
        char* name = malloc(BUFSIZ); /* creating a buffer to create a pathname to file in new folder */
        int currentNameSize = BUFSIZ;

        int dirLen = strlen(fileArr[fileCount - 1]);

        for (int i = 0; i < fileCount - 1; i++)
        {
            int pathLen = strlen(fileArr[i]);
            name = getNewFilePath(fileArr[fileCount - 1], dirLen, fileArr[i], pathLen, name, currentNameSize);
            //printf("got name %s\n", name);
            if (currentNameSize == 0)
            {
                fprintf(stderr, "the filename of %s is too big\n", name);
                currentNameSize = BUFSIZ;
                continue;
            }

            int result = fileReadWrite(fileArr[i], name, flagState); /* write from old file to new one */
        }

    free(name);

    return 0;
}

//------------------------------------------------------------------------------------------------------------------------

int mycp(int argcount, char* arguments[])
{
    if (argcount < 3)
    {
        fprintf(stderr, "Not enough arguments\n");
        return FAILURE;
    }
    Flags flagState = { 0 };

    int newFlag = 0;

    while ((newFlag = getopt_long (argcount, arguments, flags, longopts, NULL)) != -1)
    switch (newFlag)
    {
        case 'f':
            flagState.fflag = 1;
            flagState.iflag = 0;
            break;
        case 'i':
            flagState.fflag = 0;
            flagState.iflag = 1;
            break;
        case 'v':
            flagState.vflag = 1;
            break;
    
    }
    if (argcount - optind < 2)
    {
        fprintf(stderr, "You did not give enough arguments");
        return FAILURE;
    }

    int result;
    int lastDir = checkIfDir(arguments[argcount - 1]);
    printf("is last dir - %d\n", lastDir);
    if (lastDir > 0)
    {
        //printf("working with dir\n");
        result = workWithDir(arguments + optind, argcount - optind, flagState);
    }
    else if (lastDir == 0)
    {
        if (argcount - optind > 2)
        {
            fprintf(stderr, "wrong use: more than two arguments but last one is not a directory\n");
            return FAILURE;
        }
        else
            result = fileReadWrite( arguments[optind], arguments[optind + 1], flagState);
    }
    else
    {
        fprintf(stderr, "there apperead a problem with writing to last directory: you don't have rights\n");
    }

    return result;
}

int main (int argc, char* argv[])
{
    mycp(argc, argv);

    return 0;
}