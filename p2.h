#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h> // used for killpg()
#include <fcntl.h> // used for dup2()


//#include "/home/cs/faculty/cs570/CHK.h" // Used for CHK() function to wait for pid
#include "CHK.h"
#include "getword.h"
#include <dirent.h> // Used for opendir() to check for file existence
#include <errno.h> // used for opendir() to check for file existence

#define MAXITEM 100 // max number of words per line
#define MAXINPUT 10000 // max number of chars in input.
