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
char *argv1[MAXITEM];         // array of pointers to word locations for argv child 1
char *argv2[MAXITEM];         // array of pointers to word locations for argv child 2

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
char writeLocationApp[STORAGE]; // write location append flag and pointer
char writeLocationBack[STORAGE]; // write location for >>&
char writeLocationAnd[STORAGE];  // write location for >&
char rawInput[MAXINPUT];
int EOFDetected = 0;
char *argvCopy[];
int argcCopy;

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
    *writeLocationApp = '\0';
    *writeLocationBack = '\0';
    *writeLocationAnd = '\0';
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

void formatPipeArgv() {
    // Set up the argv1 and argv2 arrays for piping
    char **wordLocationsPointer = wordLocations;
    
    int i = 0;
    while (**(wordLocationsPointer) != '|') {
        argv1[i] = *(wordLocationsPointer++);
        i++;
    }
    wordLocationsPointer++;
    argv1[i] = NULL;
    i++;
    int j = 0;
    while (i < numWords) {
        argv2[j] = *(wordLocationsPointer++);
        j++;
        i++;
    }
    argv2[j] = NULL;
}

main(int argc, char *argv[])
{
    argcCopy = argc;
    if (argc >= 2) {
        argvCopy[1] = argv[1];
    }
    char **wordLocationsPointer = wordLocations; //use *(wordLocationsPointer) to access
    pid_t child;
    setpgid(0,0);
    (void) signal(SIGTERM, myhandler);

    for(;;) {
        // did the file we are reading frome end?

        if (EOFDetected) {
            break;
        }
        
        printf("%%%d%% ", numOfCommands);

        parse(rawInput, 0);

        /* check for Done */
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
                
                if (chdir(*(wordLocationsPointer + 1)) != 0) {
                    
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
                    if (chdir(tempBuff) != 0) {
                        perror("could not cd");
                    }
                    
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
            
            fflush(NULL); //clear output so no duplicates are printed
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
                
                /* Check for >& to write into file and redirect*/
                if (*writeLocationAnd != '\0') {
                    if (access(writeLocationAnd, F_OK) != -1) {
                        perror("Cannot write, file exists\n");
                        exit(1);
                    }
                    int exists = open(writeLocationAnd, O_RDWR | O_CREAT, S_IRUSR | S_IWUSR);
                    int dup2Out = dup2(exists, STDOUT_FILENO);
                    int dup2Out1 = dup2(exists, STDERR_FILENO);
                    close(exists);
                    
                }
                
                /* Check for >> to append to file */
                if (*writeLocationApp != '\0') {
                    if (access(writeLocationApp, F_OK) == -1) {
                        perror("Cannot write, file does not exist\n");
                        exit(1);
                    }
                    int exists = open(writeLocationApp, O_APPEND | O_RDWR | S_IRUSR | S_IWUSR);
                    int dup2Out = dup2(exists, STDOUT_FILENO);
                    close(exists);
                }
                
                /* Check for >>& to append and redirect */
                if (*writeLocationBack != '\0') {
                    if (access(writeLocationBack, F_OK) == -1) {
                        perror("Cannot write, file does not exist\n");
                        exit(1);
                    }
                    int exists = open(writeLocationBack, O_APPEND | O_RDWR | S_IRUSR | S_IWUSR);
                    int dup2Out = dup2(exists, STDOUT_FILENO);
                    int dup2Out1 = dup2(exists, STDERR_FILENO);
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
            fflush(NULL); //clear output so no duplicates are printed
            
            //format argv1 and argv2 arrays
            formatPipeArgv();
            
            //set up file read write

            
            
            pid_t first, second;
            // create first child
            CHK(first = fork());
            if (0 == first) {
                int fildes[2];
                pipe(fildes);
                
                //create second child
                CHK(second = fork());
                if (second == 0) {
                    CHK(dup2(fildes[1],STDOUT_FILENO));
                    CHK(close(fildes[0]));
                    CHK(close(fildes[1]));
                    
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
                    
                    // run commands
                    if (execvp(argv1[0], argv1) != 0) {
                        perror("The program could not be executed\n");
                        exit(2);
                    }
                    
                }
                
                //wait for second to finish
                for (;;) {
                    pid_t pid;
                    CHK(pid = wait(NULL));
                    if (pid == second) {
                        break;
                    }
                }
                //once second child done, set stdout
                CHK(dup2(fildes[0],STDIN_FILENO));
                CHK(close(fildes[0]));
                CHK(close(fildes[1]));
               
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
                
                /* Check for >& to write into file and redirect*/
                if (*writeLocationAnd != '\0') {
                    if (access(writeLocationAnd, F_OK) != -1) {
                        perror("Cannot write, file exists\n");
                        exit(1);
                    }
                    int exists = open(writeLocationAnd, O_RDWR | O_CREAT, S_IRUSR | S_IWUSR);
                    int dup2Out = dup2(exists, STDOUT_FILENO);
                    int dup2Out1 = dup2(exists, STDERR_FILENO);
                    close(exists);
                    
                }
                
                /* Check for >> to append to file */
                if (*writeLocationApp != '\0') {
                    if (access(writeLocationApp, F_OK) == -1) {
                        perror("Cannot write, file does not exist\n");
                        exit(1);
                    }
                    int exists = open(writeLocationApp, O_APPEND | O_RDWR | S_IRUSR | S_IWUSR);
                    int dup2Out = dup2(exists, STDOUT_FILENO);
                    close(exists);
                }
                
                /* Check for >>& to append and redirect */
                if (*writeLocationBack != '\0') {
                    if (access(writeLocationBack, F_OK) == -1) {
                        perror("Cannot write, file does not exist\n");
                        exit(1);
                    }
                    int exists = open(writeLocationBack, O_APPEND | O_RDWR | S_IRUSR | S_IWUSR);
                    int dup2Out = dup2(exists, STDOUT_FILENO);
                    int dup2Out1 = dup2(exists, STDERR_FILENO);
                    close(exists);
                    
                }
                
                //run commands
                if (execvp(argv2[0], argv2) != 0) {
                    perror("The program could not be executed\n");
                    exit(2);
                }
            }
            
            //wait for first to finish
            for (;;) {
                pid_t pid;
                CHK(pid = wait(NULL));
                if (pid == first) {
                    break;
                }
            }
        }
        

    }
    killpg(getpgrp(), SIGTERM); // Terminate any children that are still running. 
    if (EOFDetected == 0) printf("p2 terminated.\n"); // ensure printf is after killpg
    exit(0);

}

char * getLine()
{
    int character;
    int c;
    c = 0;
    do
    {
        
        if ((character = getchar()) == EOF) {
            EOFDetected = 1;
        }
        
        rawInput[c]   = character;
        c++;
    }
    while(character != '\n' && EOFDetected == 0);
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
    if (argcCopy >= 2 && argvCopy[1] != NULL) {
          printf("%s \n", argvCopy[1]);   
    /* Check for < to read from file */
            if (*argvCopy[1] != '\0')  {
                if (access(argvCopy[1], R_OK) == -1) {
                    perror("Cannot read, file does not exist\n");
                    exit(2);
                }
                int exists = open(argvCopy[1], O_RDONLY);
                int dup2Out = dup2(exists, STDIN_FILENO);
                close(exists);
                argcCopy = 1;
            }
    }
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
        /* Set writeLocationAnd if >& is detected */
        if (*lineInputPointer == '>' && *(lineInputPointer+1) == '&' && wordSize == 2) {
            wordSize = getword(writeLocationAnd, pointerToRawInputPointer);
            if (wordSize == 0) {
                perror("Error, no file was specified to write\n");
                resetGlobalVariables();
                break;
            }
            continue; // <& should not be counted as a word
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
        // set writeLocationApp if >> is detected
        if (*lineInputPointer == '>' && *(lineInputPointer+1) == '>' && wordSize == 2) {
            wordSize = getword(writeLocationApp, pointerToRawInputPointer);
            if (wordSize == 0) {
                perror("Error, no file was specified to write append\n");
                resetGlobalVariables();
                break;
            }
            continue; // < should not be counted as a word
        }
        // set writeLocationBack if >>& is detected
        if (*lineInputPointer == '>' && *(lineInputPointer+1) == '>' && *(lineInputPointer+2) == '&' & wordSize == 3) {
            wordSize = getword(writeLocationBack, pointerToRawInputPointer);
            if (wordSize == 0) {
                perror("Error, no file was specified to write\n");
                resetGlobalVariables();
                break;
            }
            continue; // <<& should not be counted as a word
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
