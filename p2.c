#include "getword.h"
#include "p2.h"

/* Caleb Christiansen
 * John Carroll
 * CS570
 * Program 4
 * Due Nov 29, 2019 11/29/2019
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
int pipeFlag = 0;
char writeLocation[STORAGE];  // storage for the file to write to.
char readLocation[STORAGE];   // storage for the file to read from.
char rawInput[MAXINPUT];

void myhandler(int signum) // not sure what this is for yet
{
    //printf("Recieved SIGTERM (%d), and the special handler is running...\n", signum);
    complete = 1;

}

void copyString(char * sToCopy, char * sSaveLocation, int size)
{
    //copys an array of args to another array
    int i = 0;
    for (i = 0; i < size; i++) {
        *sSaveLocation = *sToCopy;
        sSaveLocation++;
        sToCopy++;
    }
    
}

void saveToHistory(int instructNum)
{
    // After each command, save the array to its input location
    if (instructNum == 1) {
        copyString(rawInput, Input1, MAXINPUT);
    } else if (instructNum == 2) {
        copyString(rawInput, Input2, MAXINPUT);
    } else if (instructNum == 3) {
        copyString(rawInput, Input3, MAXINPUT);
    } else if (instructNum == 4) {
        copyString(rawInput, Input4, MAXINPUT);
    } else if (instructNum == 5) {
        copyString(rawInput, Input5, MAXINPUT);
    } else if (instructNum == 6) {
        copyString(rawInput, Input6, MAXINPUT);
    } else if (instructNum == 7) {
        copyString(rawInput, Input7, MAXINPUT);
    } else if (instructNum == 8) {
        copyString(rawInput, Input8, MAXINPUT);
    } else if (instructNum == 9) {
        copyString(rawInput, Input9, MAXINPUT);
    } else if (instructNum == 10) {
        copyString(rawInput, Input10, MAXINPUT);
    }
    return;
}

void resetGlobalVariables()
{
    numWords = 0;
    doneEofFlag = 0;
    cdFlag = 0;
    complete = 0;
    *writeLocation = '\0';
    *readLocation = '\0';
    backgroundFlag = 0;
    pipeFlag = 0;
}

void clearArray(char *arrayToClear, int size)
{
    int i = 0;
    for (i = 0; i < size; i++) {
        *arrayToClear = '\0';
        arrayToClear++;
    }
}

main()
{
    char **wordLocationsPointer = wordLocations; //use *(wordLocationsPointer) to access
    pid_t child;
    setpgid(0,0);
    (void) signal(SIGTERM, myhandler);

    for(;;) {
        printf("%%%d%% ", numOfCommands);
        parse(rawInput, 0);
        
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
        
        if (pipeFlag != 1) {
        // There is no pipe, run normally
            
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
                
                /* Check for < to read from file */
                if (*readLocation != '\0')  {
                    if (access(readLocation, R_OK) == -1) {
                        perror("Cannot read, file does not exist\n");
                        exit(2);
                    }
                    int exists = open(readLocation, O_RDONLY);
                    int dup2Out = dup2(exists, STDIN_FILENO);
                    close(exists);
                }
                

                
                /* This code is ineffcient! Will move to parse() later if time permits */
                /* It is finding the arguments to place in execvp */
                char *cmd = *wordLocationsPointer;
                char *argv[MAXINPUT];
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
        else {
            // There is a pipe! run pipe code
            
        }
    }
    killpg(getpgrp(), SIGTERM); // Terminate any children that are still running. 
    printf("p2 terminated.\n"); // ensure printf is after killpg
    exit(0);

}

char * getLine()
{
    char character;
    int c;
    c = 0;
    do
    {
        character = getchar();
        rawInput[c]   = character;
        c++;
    }
    while(character != '\n');
    c = c - 1;
    rawInput[c] = '\0';
    
    return rawInput;
}

char * getHistory(int instructNum)
{
    // After each command, save the array to its input location
    if (instructNum == 1) {
        return Input1;
    } else if (instructNum == 2) {
        return Input2;
    } else if (instructNum == 3) {
        return Input3;
    } else if (instructNum == 4) {
        return Input4;
    } else if (instructNum == 5) {
        return Input5;
    } else if (instructNum == 6) {
        return Input6;
    } else if (instructNum == 7) {
        return Input7;
    } else if (instructNum == 8) {
        return Input8;
    } else if (instructNum == 9) {
        return Input9;
    } else if (instructNum == 10) {
        return Input10;
    }
    return rawInput;
}

int charToInt(char charToConvert)
{
    int x = charToConvert - '0';
    return x;
}

int parse(char *rawInputPointer, int userInputFlag)
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

    /* Reset Global Variables */
    resetGlobalVariables();

    /* Prompt User for Input */
    if (userInputFlag == 0) {
        clearArray(rawInput, MAXINPUT);
        rawInputPointer = getLine();
    } else {
        rawInputPointer = getHistory(userInputFlag);
    }
    
    /* Store to history */
    saveToHistory(numOfCommands);
    
    // Store words into: lineInput
    // Store word locations (pointers) into: wordLocations

    while (wordSize > 0) {
        
        char **pointerToRawInputPointer = &rawInputPointer;
        wordSize = getword(lineInputPointer, pointerToRawInputPointer);
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
            while (getword(prevInputPointer, pointerToRawInputPointer) > 0);
            break;
        }
        /* Call history if necesary */
        if (numWords == 0 && wordSize == 2 && (*lineInputPointer == '!')){
            int historyNumber = charToInt(*(lineInputPointer+1));
            if (historyNumber < numOfCommands && historyNumber > 0) {
                parse(*pointerToRawInputPointer, historyNumber);
                break;
            } else {
                printf("ERROR, out of history range \n");
                break;
            }
        }
        /* Set writeLocation pointer if > is detected */
        if (*lineInputPointer == '>' && wordSize == 1) {
            wordSize = getword(writeLocation, pointerToRawInputPointer);
            if (wordSize == 0) {
                perror("Error, no file was specified to write\n");
                resetGlobalVariables();            
                break;
            }
            continue; // > should not be counted as a word
        }
        /* Set readLocation pointer if < is detected */
        if (*lineInputPointer == '<' && wordSize == 1) {
            wordSize = getword(readLocation, pointerToRawInputPointer);
            if (wordSize == 0) {
                perror("Error, no file was specified to read\n");
                resetGlobalVariables();
                break;
            }
            continue; // < should not be counted as a word
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
    if (numWords > 0 && userInputFlag == 0) {
        numOfCommands++;
    }
}
