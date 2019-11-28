#include "getword.h"
#include "p2.h"

/* Caleb Christiansen
 * John Carroll
 * CS570
 * Program 2
 * Due Oct 9, 2019 10/9/2019
 * This program is designed to emulate a real command shell. It can run programs and allows you
 * to navigate the file system, and manipulate files. Note that certain behavior is not exactly
 * like the shell. This is 100% intentional as this program's behavoir makes more sense. ;)
 */

/* Global Variables  */
char lineInput[MAXINPUT];
char prevInput[MAXINPUT];
char Input1[MAXINPUT];
char Input2[MAXINPUT];
char Input3[MAXINPUT];
char Input4[MAXINPUT];
char Input5[MAXINPUT];
char Input6[MAXINPUT];
char Input7[MAXINPUT];
char Input8[MAXINPUT];
char Input9[MAXINPUT];
char Input10[MAXINPUT];
char *wordLocations[MAXITEM]; // Each item is a pointer to the start of a word.
                              // access them by creating a pointer: char **pointer
                              // access pointer: *(pointer) or *(pointer + n)
int numWords;                 // The number of words on the line
int doneEofFlag = 0;          // 0 if clear, -1 if getWord returned -1
int cdFlag = 0;               // 0 if no cd, 1 if cd is present
int repeatFlag = 0;           // 0 if no !!, 1 if !! is first word on line
int complete = 0;             // 0 if signum not detected
int backgroundFlag = 0;       // 0 if wait, 1 if background
int numOfCommands = 1;        // How many successful commands have been typed?
char writeLocation[STORAGE];  // storage for the file to write to. 


void myhandler(int signum) // not sure what this is for yet
{
    //printf("Recieved SIGTERM (%d), and the special handler is running...\n", signum);
    complete = 1;

}

void copyArgs(char * sToCopy, char * sSaveLocation, int numArgs)
{
    //copys an array of args to another array
    for (int i = 0; i < numArgs; i++) {
        while (*(sSaveLocation++) = *(sToCopy++));
    }
    
}

void saveToHistory(int instructNum)
{
    // After each command, save the array to its input location
    if (instructNum == 1) {
        copyArgs(lineInput, Input1, numWords);
    } else if (instructNum == 2) {
        copyArgs(lineInput, Input2, numWords);
    } else if (instructNum == 3) {
        copyArgs(lineInput, Input3, numWords);
    } else if (instructNum == 4) {
        copyArgs(lineInput, Input4, numWords);
    } else if (instructNum == 5) {
        copyArgs(lineInput, Input5, numWords);
    } else if (instructNum == 6) {
        copyArgs(lineInput, Input6, numWords);
    } else if (instructNum == 7) {
        copyArgs(lineInput, Input7, numWords);
    } else if (instructNum == 8) {
        copyArgs(lineInput, Input8, numWords);
    } else if (instructNum == 9) {
        copyArgs(lineInput, Input9, numWords);
    } else if (instructNum == 10) {
        copyArgs(lineInput, Input10, numWords);
    }
    return;
}

void resetGlobalVariables() {
    numWords = 0;
    doneEofFlag = 0;
    cdFlag = 0;
    complete = 0;
    *writeLocation = '\0';
    backgroundFlag = 0;
}

main()
{
    char **wordLocationsPointer = wordLocations; //use *(wordLocationsPointer) to access
    pid_t child;
    setpgid(0,0);
    (void) signal(SIGTERM, myhandler);

    for(;;) {
        printf("%%%d%% ", numOfCommands);
        parse();
        
        /* check for EOF */
        if (doneEofFlag == -1 && numWords == 0) break;
        /* check for empty line */ 
        if (numWords == 0) continue;
        
        /* check for cd */
        if (cdFlag != 0) {
            /* Check for too many arguments */
            if (numWords > 2) {
                perror("Error: Too many arguments");
            }
            /* Check for just cd */
            else if (numWords == 1) {
                chdir(getenv("HOME"));
            }
            /* Check for cd pathname */
            else if (numWords == 2) {
                /* Check if dir exists */
                DIR* dir = opendir(*wordLocationsPointer + 1);
                if (dir) {
                    /* Directory exists. */
                    closedir(dir);
                    chdir(*(wordLocationsPointer + 1));
                } else if (ENOENT == errno) {
                    /* Dir doesn't exist, check for full path */
                    char tempBuff[STORAGE]; // filePath Storage
                    char * tempBuffPointer = tempBuff; 
                    char * filePointer = *(wordLocationsPointer + 1);   //name of file to cd into
                    getcwd(tempBuff, sizeof(tempBuff)); //places current working dir in tempBuff
                    
                    /* point tempBuffPointer at end of cwd */
                    while (*(tempBuffPointer++));
                    tempBuffPointer--;  // remove null terminator
                    *(tempBuffPointer++) = '/';
                   
                    /* coppy folder name onto end of cwd */
                    while (*(tempBuffPointer++) = *(filePointer++));
                    /*Check again for correct file */
                    dir = opendir(tempBuff);
                    if (dir) {
                        closedir(dir);
                        chdir(tempBuff);
                    } else if (ENOENT == errno) {
                        /* Directory does not exist. */
                        perror("This directory does not exist");
                    } else {
                        perror("The directory failed to open");
                    }
                } else {
                    /* opendir() failed to open for another reason. */
                    perror("The directory failed to open");
                }
            }
            continue; //end of cd section, reprompt
        }

        /* check for !! */
        if (repeatFlag != 0) {
                //currently handled by parse. Is this efficient?
        }
        fflush(NULL); //not sure if forcing data out is necessary. 
        /* fork a child to run the requested program */
        child = fork();
        if (child == 0) {
            // Child is doing this part
            /* redirect stdin to /dev/null */
            //int stdIn = open("/dev/null", O_RDONLY | O_RDWR | O_RDWR);
            //int dup2In = dup2(stdIn, STDIN_FILENO);

            /* Check for > to write into file */
            if (*writeLocation != '\0')  {
                if (access(writeLocation, F_OK) != -1) {
                    perror("Cannot write, file already exists\n");
                    exit(1);
                }
                int exists = open(writeLocation, O_RDWR | O_CREAT, S_IRUSR | S_IWUSR);
                int dup2Out = dup2(exists, STDOUT_FILENO);
                close(exists);
            }

            
            /* This code is ineffcient! Will move to parse() later if time permits */
            /* It is finding the arguments to place in execvp */
            char *cmd = *wordLocationsPointer;
            char *argv[numWords];
            int i;
            for (i = 0; i <= numWords; i++) {
                argv[i] = *(wordLocationsPointer++);
                if (i == numWords) {
                    argv[i] = NULL;
                }
            }
            /* execute the program! */
            if (execvp(cmd, argv) != 0) {
                perror("The program could not be executed\n");
                exit(2);
            }
        }

        /* wait for child */ 
        for (;;) {
            pid_t pid;
            if (backgroundFlag) {
                printf("pid = [%d]\n", child);
                printf("Process = %s\n", (*wordLocations));
                break;
            }
            CHK(pid = wait(NULL));
            if (pid == child) {
                break;
            }
        }
    }
    killpg(getpgrp(), SIGTERM); // Terminate any children that are still running. 
    printf("p2 terminated.\n"); // ensure printf is after killpg
    exit(0);

}

int parse()
{
    /* Local Variables */
    int wordSize = 1;
    char *lineInputPointer = lineInput;
    char *prevInputPointer = prevInput;
    char **wordLocationsPointer = wordLocations;
    
    /* Copy first word of prev line/ backup global vars */
    while (*(prevInputPointer++) = *(lineInputPointer++));
    lineInputPointer = lineInput; 
    int cdFlagPrev = cdFlag;
    int numWordsPrev = numWords;
    int repeatFlagPrev = repeatFlag;
    
    /* Store to history */
    saveToHistory(numOfCommands);

    /* Reset Global Variables */
    resetGlobalVariables();

    // Store words into: lineInput
    // Store word locations (pointers) into: wordLocations
    while (wordSize > 0) {
        
        wordSize = getword(lineInputPointer);
        *wordLocationsPointer = lineInputPointer;
        /* Check for done special case: wordSize is -1 if done is first word */
        if (wordSize == -1 && (*lineInputPointer) == 'd' && numWords > 0) {
                wordSize = 4;
        }
        /* Set doneEofFlag if done or  EOF is detected */
        if (wordSize == -1) {
             doneEofFlag = wordSize;
        }
        /* Set cdFlag if cd is detected */
        if (numWords == 0 && wordSize == 2 && (*lineInputPointer) == 'c' && (*lineInputPointer+1) == 'd') {
            cdFlag = 1;
        }
        /* Set repeatFlag if !! is the first word */
        if (numWords == 0 && wordSize == 2 && (*lineInputPointer == '!') && (*(lineInputPointer+1) == '!')) {
            repeatFlag = 1;
            lineInputPointer = lineInput;
            prevInputPointer = prevInput;
            numWords = numWordsPrev;
            cdFlag = cdFlagPrev;
            while (*(lineInputPointer++) = *(prevInputPointer++));
            lineInputPointer = lineInput;
            /* Throw away the rest of the line */
            while (getword(prevInputPointer) > 0);
            break;
        }
        /* Set writeLocation pointer if > is detected */
        if (*lineInputPointer == '>' && wordSize == 1) {
            wordSize = getword(writeLocation);
            if (wordSize == 0) {
                perror("Error, no file was specified\n");
                resetGlobalVariables();            
                break;
            }
            continue; // > should not be counted as a word
        }
        /* Check for background command (&) */
        if (wordSize == 1 && *lineInputPointer == '&') {
            backgroundFlag = 1;
        }
        /* & is not the last word */
        else if (wordSize > 0) {
            backgroundFlag = 0;
        }
        /* increment pointers and numWords (don't increment if the terminate signal -1 is given) */
        if (wordSize > 0) {
            lineInputPointer = lineInputPointer + wordSize + 1;
            numWords++;
            wordLocationsPointer++;
        }
    }
    /* delete & if necessary */
    if (backgroundFlag == 1) {
        *(wordLocations+numWords-2) = '\0';
        numWords--;
    }
    if (numWords > 0) {
        numOfCommands++;
    }
}




