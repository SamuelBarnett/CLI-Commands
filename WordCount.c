/*======================================================================
|
|
|   Name: WordCount.c
|
|   Written by: Samuel Barnett - October 2022
|
|   Purpose: To replicate the function of the wc command as close as possible
|
|   Compile: gcc -o WordCount.exe WordCount.c -std=c99
|
|   usage:  ./WordCount.exe file.txt
|
|   Description of parameters:
|     file.txt - A text file.
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
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <malloc.h>
// Sets the buffer size
#define BUFFERSIZE 1024

// Defines a structure for the variables needed in word count
struct WordCount
{
    int newLine;
    int word;
    int bytes;
};
// Count the new lines, words and bytes in a given file
int wordCount(struct WordCount *WC, int FILE); 

// Main accepts command line arguments
int main(int argc, char *argv[])
{
    // instantiates word count struct
    struct WordCount WC;
    // Checks if the correct amount of arguments where entered, else it exits the program.
    if (argc != 2)
    {
        printf("%s", "Please enter the correct amount of arguments, One argument that is a text file \n");
        return 1;
    }
    // opens the file through a command line argument and uses read mode
    int FILE = open(argv[1], O_RDONLY);

    // checks if the reference to the file is null, if so it sends an error message and exits the program
    if (FILE == -1)
    {
        printf("%s", "The file you have given failed to open \n");
        return 1;
    }
    else
    {
        // Calls the wordCount function
        wordCount(&WC, FILE);
    }
    // Prints off the number of newLines
    printf("Lines: %i  ", WC.newLine);
    // Prints off the Word count
    printf("Words: %i  ", WC.word);
    // Prints off the number of bytes
    printf("bytes: %i  \n", WC.bytes);
    // returns 0 if there were no errors
    return 0;

} // end main

/*======================================================================
|
|
|   Function: wordCount
|
|   Written by: Samuel Barnett - October 2022
|
|   Purpose: Counts the new lines, words and bytes in a given file
|
|
|   Description of parameters:
|    struct WordCount *WC - pointer to a structure containing variables for new line, bytes and words.
|    int FILE the return value of an open() function 
|
|   Subroutines/libraries required:
|      none.
|
|
|------------------------------------------------------------------*/
int wordCount(struct WordCount *WC, int FILE)
{

    // creates a buffer for the file
    char Buffer[BUFFERSIZE];
    
    //initialize variables
    WC->word = 0;
    WC->newLine = 0;
    WC->bytes = 0;
    // Meant to be the output of read() for the file, set as the expected full buffer
    // to set a working condition on while loop.
    int readFile = 1024;
    // reads off information from the pointer to the file and stores the count of bytes into the variable 'count'
    while (readFile == 1024)
    {
        // Reads off the file 1024 bytes at a time
        readFile = read(FILE, Buffer, 1024);
        // If the file returns -1 it prints an error message
        if (readFile == -1)
        {
            printf("%s", "The file you have given failed to read");
            return 1;
        }

        // uses a for loop to traverse through the buffer which contains the content read off of the file.
        // It counts as a word if there is a space and the character after the is not a space. Only counts
        // as many bytes that the current read() output has given.
        for (int i = 0; i < readFile; i++)
        {
            // If statement for considering if the current character in the file is a word seperator or not.
            if (isspace(Buffer[i]) && !isspace(Buffer[i + 1]))
            {
                // adds one word if true
                WC->word++;
            }
            // Counts if the character is a new line
            if (Buffer[i] == '\n')
            {
                // adds one new line to the total if true
                WC->newLine++;
            }
        }

        // Sets the bytes of the WC struct to what read() returned.
        // if the the while loops repeats it adds the rest to the
        // total count
        WC->bytes += readFile;

    }

    // Closes the file
    close(FILE);

    // Returns 0 if no errors occurred
    return 0;
}
