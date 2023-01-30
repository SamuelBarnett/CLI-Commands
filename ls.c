/*======================================================================
|
|
|   Name: FileSTATs.c
|
|   Written by: Samuel Barnett - October 2022
|
|   Purpose: To replicate the function of the ls -l command as close as possible
|
|   Compile: gcc -o FileSTATs.exe FileSTATs.c
|
|   usage:  ./FileSTATs.exe sampleDirectory
|
|   Description of parameters:
|     sampleDirectory - A path to a directory.
|
|
|   Subroutines/libraries required:
|
|      See include statements.
|
|
|------------------------------------------------------------------*/
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>
#include <time.h>

#define ERROR -1
#define NOERROR 0
#define BUFFERSIZE 1024
#define INT_MAX 2147483647

// defines function to print in ls -l format
int LSFormat(struct stat *Info, char *selectedFile);

int main(int argc[], char *argv[])
{
    // pointer to directory
    DIR *dp;
    // structures to hold dirent and stat buffers.
    struct dirent *dir;
    struct stat Info;
    // variables required to find the largest/smallest file and most
    // recently modified and oldest modified.
    // 500 array chosen because it is over the file path limit of 260
    int largestFile = 0;
    int smallest = INT_MAX;
    char largestName[500];
    char smallestName[500];
    int leastRecent = 0;
    int mostRecent = INT_MAX;
    char mostRecentName[500];
    char leastRecentName[500];
    int count = 0;
    char forwardSlash[10] = "/";

    // If there is one one argument then the user did not specify a file and it
    // runs the script in the current directory
    if (argc == 1)
    {
        // opens current directory
        dp = opendir(".");
        // If opendir returns null or 0 then couldn't open file and return error
        if (dp == NULL || dp == 0)
        {
            printf("Filed to open file");
            return ERROR;
        }
    }
    // if there is an argument then it takes the argument and opens that directory
    else if (argc == 2)
    {
        // opens directory given by user
        if ((dp = opendir(argv[1])) == NULL)
        {
            // If opendir returns null then couldn't open file and return error
            printf("Failed to open file \n");
            return ERROR;
        }
    }
    // prints an error if the user inputted to many arguments
    else if (argc != 1 && argc != 2)
    {
        printf("Please enter the correct number of arguments. \n");
        return ERROR;
    }
    // Find the directory with the largest size, smallest size, most recently modified  and latest modified and find the names of them.
    // Put those names in an array and send them to LSformat where they will be stat called again and printed in LS format.
    while ((dir = readdir(dp)) != NULL)
    {
        // sets the base path to ./ to check the current directory
        char pathBuffer[500] = "./";
        // If the user entered a specific argument then it runs that as the path
        if (argc == 2)
        {
            strncpy(pathBuffer, argv[1], sizeof(argv[1]));
            //adds the / required in path
            strncat(pathBuffer, forwardSlash, 450);
        }
        // Copies the current file onto the end of the path
        strncat(pathBuffer, dir->d_name, 450);
        // calls stat on the current file
        stat(pathBuffer, &Info);
        // checks if the file is a directory and if the file has a valid i-node number.
        if ((S_ISDIR(Info.st_mode) != 1) && (dir->d_ino != 0))
        {
            count++;
            // if the size of the file is greater than the current largest then it becomes the new largest
            if (Info.st_size > largestFile)
            {
                largestFile = Info.st_size;
                strncpy(largestName, pathBuffer, sizeof(pathBuffer));
            }
            // if the size of the file is greater than the current smallest then it becomes the new smallest
            if (Info.st_size < smallest)
            {
                smallest = Info.st_size;
                strncpy(smallestName, pathBuffer, sizeof(pathBuffer));
            }
            // if the time is greater than the current largest then it becomes the least Recent
            if (Info.st_mtime > leastRecent)
            {
                leastRecent = Info.st_mtime;
                strncpy(mostRecentName, pathBuffer, sizeof(pathBuffer));
            }
            // if the time is less than the current smallest then it becomes the most Recent
            if (Info.st_mtime < mostRecent)
            {
                mostRecent = Info.st_mtime;
                strncpy(leastRecentName, pathBuffer, sizeof(pathBuffer));
            }
        }
        bzero(pathBuffer, sizeof(pathBuffer));
    }
    // if no files were read then it sends an error
    if (count == 0)
    {
        printf("There are no valid files to read. \n");
        return ERROR;
    }
    // Prints the files in ls format by calling the LSformat function.
    printf("Largest File: \n");
    LSFormat(&Info, largestName);
    printf("Smallest File: \n");
    LSFormat(&Info, smallestName);
    printf("Most Recently Modified File: \n");
    LSFormat(&Info, mostRecentName);
    printf("Oldest Modified File: \n");
    LSFormat(&Info, leastRecentName);

    closedir(dp);

    return NOERROR;
};
/*======================================================================
|
|
|   Function: LSFormat
|
|   Written by: Samuel Barnett - October 2022
|
|   Purpose: Prints file info in ls -l format
|
|
|   Description of parameters:
|    struct stat *Info - pointer to a structure containing variables for the file info.
|    char *selectedFile - The file selected to print.
|
|   Subroutines/libraries required:
|      none.
|
|
|------------------------------------------------------------------*/
int LSFormat(struct stat *Info, char *selectedFile)
{
    // if the file failed to pass it returns an error
    if (selectedFile == NULL)
        return ERROR;
    // buffer to print the file name
    char nameBuff[300];
    char fileBuff[300];
    int i;
    // copies the selected file into the filebuff
    for (i = 0; i < sizeof(fileBuff); i++)
    {
        fileBuff[i] = selectedFile[i];
    }
    // calls stat on the selected file
    stat(fileBuff, Info);

    // turnery operator formatting idea flagrantly stolen from https://stackoverflow.com/questions/10323060/printing-file-permissions-like-ls-l-using-stat2-in-c
    // Uses the turnery operator to print the file permissions in the same format as ls -l
    printf((S_ISDIR(Info->st_mode)) ? "d" : "-");
    // permissions for user
    printf((Info->st_mode & S_IRUSR) ? "r" : "-");
    printf((Info->st_mode & S_IWUSR) ? "w" : "-");
    printf((Info->st_mode & S_IXUSR) ? "x" : "-");
    // permissions for group
    printf((Info->st_mode & S_IRGRP) ? "r" : "-");
    printf((Info->st_mode & S_IWGRP) ? "w" : "-");
    printf((Info->st_mode & S_IXGRP) ? "x" : "-");
    // permissions for others
    printf((Info->st_mode & S_IROTH) ? "r" : "-");
    printf((Info->st_mode & S_IWOTH) ? "w" : "-");
    printf((Info->st_mode & S_IXOTH) ? "x" : "-");
    printf("   ");
    // prints the other ls -l information
    printf(" %o  ", Info->st_nlink);
    printf(" %o  ", Info->st_uid);
    printf(" %o  ", Info->st_gid);
    printf(" %o  ", Info->st_size);
    printf(" %s", ctime(&Info->st_mtime));

    // Finds the name of the file by deleting the old path when a / occurs
    for (i = 0; i < sizeof(fileBuff); i++)
    {
        nameBuff[i] = fileBuff[i];
        if (fileBuff[i] == '/')
        {
            bzero(nameBuff, sizeof(nameBuff));
        }
    }
    // prints off name
    for (i = 0; i < sizeof(nameBuff); i++)
    {
        printf("%c", nameBuff[i]);
    }
    printf("\n");
    return NOERROR;
};
