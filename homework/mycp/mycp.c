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
    STOPPED,
    NON_EXIST,
    SUCCESS = 0,
    EXISTS = 1
};

typedef struct FlagStruct
{
    int fflag;
    int vflag;
    int iflag;
}Flags;

int mycp(int argcount, char* arguments[]);
int checkIfExists (char* Filename);
int ReadWrite(char* FileFrom, char* FileTo, int mode, Flags FlagState);
int myopen(char* FileName, int mode, Flags FlagState);
char* GetNewFilePath (char* dirpath, size_t dirLen, char* OldFilePath, int pahtLen, char* NewFilePath);
int WorkWithDir (char* FileArr[], int Filecount, Flags FlagState);


//------------------------------------------------------------------------------------------------------------------------

int checkIfExists(char* Filename)
{
    if( access( Filename, F_OK ) == 0 )
    {
        return EXISTS;
    }
    else
    {
        return NON_EXIST;
    }
}

//------------------------------------------------------------------------------------------------------------------------

int WorkWithDir (char* FileArr[], int Filecount, Flags FlagState)
{

        DIR* folder = opendir(FileArr[Filecount - 1]); // just for checking whether this is a viable directory

        if (folder == NULL)
        {
            perror("Could not open your directory");
            return 0;
        }

        closedir(folder);

        int dirLen = strlen(FileArr[Filecount - 1]);
        char* Name = (char*)calloc(BUFSIZ, sizeof(char*)); /* creating a buffer to create a pathname to file */
                                                           /* that should be created */
        int currentNameSize = BUFSIZ;
        for (int i = 0; i < Filecount - 1; i++)
        {
            int pathLen = strlen(FileArr[i]);

            if (pathLen + dirLen > currentNameSize)
            {
                Name = (char*)realloc(Name, (pathLen + dirLen + 1) * sizeof(char)); /* if path are too long - reallocate */
                currentNameSize = pathLen + dirLen + 1;
            }

            Name = GetNewFilePath(FileArr[Filecount - 1], dirLen, FileArr[i], pathLen, Name);

            int Result = ReadWrite(FileArr[i], Name, 0, FlagState); /* write from old file to new one */
            if (Result == SUCCESS && FlagState.vflag == 1)
            {
                printf("%s -> %s\n", FileArr[i], Name); /* verbose flag mode */
            }

            memset(Name, 0, strlen(Name));  /* clear the buffer for new file path for the next file */
        }

    free(Name);

    return 0;
}

//------------------------------------------------------------------------------------------------------------------------

char* GetNewFilePath (char* dirpath, size_t dirLen, char* OldFilePath, int pathLen, char* NewFilePath)
{
    NewFilePath = strcpy(NewFilePath, dirpath); /* copy path to directory where w=the file should be created */

    if ( dirpath[dirLen - 1] != '/' ) /* add a '/' after the directory name, if it is not there */
        {
            NewFilePath[dirLen] = '/';
            NewFilePath[dirLen + 1] = '\0';
        }

    int fileLen = 0;                                                /* get the lenth of the file name: start from the */
    for (int j = pathLen; (j >= 0) && (OldFilePath[j] != '/'); j--) /* end of path and count till a '/' is encountered or */
        fileLen++;                                                  /* the path has ended */

    //printf("\nGot a new file path - %s\n", FileArr[i] + (pathLen) - fileLen + 1);
    NewFilePath = strcat(NewFilePath, (OldFilePath + pathLen - fileLen + 1)); /* get a new file path by catting the file name to the dir path */

    return NewFilePath;
}


//------------------------------------------------------------------------------------------------------------------------

int myopen(char* FileName, int mode, Flags FlagState)
{
    assert ( (mode == READ) || (mode == WRITE)); /* check whether func is used properly  */

    if (mode == WRITE)
    {
        int existFlag = checkIfExists(FileName);
        if (FlagState.iflag && (existFlag == EXISTS )) /* interactive flag opening of the file mode */
            {
                char UserAnswer = 0; /* char for getting user answer */

                printf("overwrite %s? (y/n [n]) ", FileName);

                while ( (UserAnswer = getchar()) == '\n') /* emptying the input buffer from previous '/n' */
                ;
                if ( tolower(UserAnswer) != 'y' )
                {
                    printf("not overwritten\n");
                    return STOPPED;
                }
    
            }

        int fd = 0;
        
        if (existFlag == EXISTS)
        {
            fd = open(FileName, O_WRONLY | O_TRUNC);

            if ( (fd < 0))
            {
                if (FlagState.fflag) /* force opening of file */
                {
                    remove(FileName); 
                    fd = open (FileName, O_WRONLY | O_CREAT);
                }
                else /* if flag for force was not given and can't open a file print the error and end the opening */
                {
                    printf("\n\nWarning! File %s\n\n", FileName);
                    perror("There apperead a problem with opening the file");
                    close(fd);
                    return FAILURE;
                }
            }

            if (fd < 0) /* if could not create a file after force flag */
            {
                printf("Could not open your file and could not create file with the same name. Sorry\n");
                return FAILURE;
            }
        }

        else /* if the file with given name doesn't exist */
        {
            fd = open(FileName, O_WRONLY | O_CREAT);
            if (fd < 0)
            {
                printf("Could not create file with the name %s\n", FileName);
                perror("Sorry");
                return FAILURE;
            }
        }

        return fd;

    }


    else
    {    
        int fd = open(FileName, O_RDONLY);

        if (fd < 0)
            {
                printf("\n\nWarning! File %s\n\n", FileName);
                perror("There apperead a problem with opening the file");
                close(fd);
                return FAILURE;
            }
        return fd;
    }
}

//------------------------------------------------------------------------------------------------------------------------

int ReadWrite(char* FileFrom, char* FileTo, int mode, Flags FlagState)
{
    char buffer [BUFSIZ] = { 0 };

    int fdFrom = myopen(FileFrom, READ, FlagState);
    int fdTo = myopen(FileTo, WRITE, FlagState);

    if (fdFrom == FAILURE || fdTo == FAILURE)
        return FAILURE;

    if (fdTo == STOPPED)
        return STOPPED;

    while(1)
    {
        int ReadFlag = 0;
        int LeftToRead = BUFSIZ;
        while (1)
        {
            ReadFlag = read(fdFrom, buffer + ReadFlag, BUFSIZ);
            if (ReadFlag < 0)
            {
                perror("There apperead a problem with reading the file: ");
                close(fdFrom);
                return FAILURE;
            }
            LeftToRead -= ReadFlag;

            if ( (ReadFlag == 0) || (LeftToRead == 0) )
                break;
        }

        int countWritten = 0;
        int LeftToWrite = BUFSIZ - LeftToRead;

        do
        {
            countWritten = write(fdTo, buffer + countWritten, LeftToWrite);
            LeftToWrite -= countWritten;
            
        }while ((countWritten != 0) && (LeftToWrite != 0));

        if (ReadFlag == 0)
            break;
    }
    close (fdFrom);
    close (fdTo);

    return SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------

int mycp(int argcount, char* arguments[])
{
    if (argcount < 3)
    {
        printf("Not enough arguments\n");
        return 0;
    }
    Flags FlagState = {0};

    int newFlag = 0;

    
    while ((newFlag = getopt_long (argcount, arguments, flags, longopts, NULL)) != -1)
    switch (newFlag)
    {
        case 'f':
            FlagState.fflag = 1;
            break;
        case 'v':
            FlagState.vflag = 1;
            break;
        case 'i':
            FlagState.iflag = 1;
            break;
    
    }
    if (argcount - optind < 2)
    {
        printf("You did not give enough arguments");
        return FAILURE;
    }

    if (argcount - optind > 2)
    {
        //printf("argcount = %d, optind = %d\n", argcount, optind);
        WorkWithDir(arguments + optind, argcount - optind, FlagState);
    }
    else
    {
        int Result = ReadWrite( arguments[optind], arguments[optind + 1], 0, FlagState);

        if (Result == SUCCESS && FlagState.vflag == 1)
        {
            printf("%s -> %s\n", arguments[optind], arguments[optind + 1]);
        }
    }

    return 0;
}

int main (int argc, char* argv[])
{
    mycp(argc, argv);

    return 0;
}