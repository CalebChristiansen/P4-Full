/*           getword.c
 * 
 * Student: Caleb Christiansen
 * Professor: Dr. John Carroll
 * Class: CS570
 * Due: September 2019 (9/2019)
 * Program 1
 *
 * Synopsis  - Fulfills the requirements of 'getword.h' The following explanation is partiallly
 *             taken From ~carroll/cs570program1.txt:
 *             getword() examines successive chars on the input stream (known as 'stdin') until
 *             getword() returns a negative number. It also saves those chars to the location 
 *             passed into the function by pointer *w  
 *
 * Objective - To build a simple lexical analyzer.  
 */

/* Include Files */
#include <stdio.h>
#include "getword.h"

const int space = ' ';
const int lengthOfDone = 3;
extern int pipeFlag;             // Is | present?


int getword(char *w, char **pointerToInput) {
    
    int iochar, n = 0;
    char *letterLocation = w;
    int isPipe = 1;
    

    while ( ( iochar = *((*pointerToInput)++) ) != '\0' ) {
        /*Check for ignoring special characters */
        if (iochar == '\\') {
            /*Grab the char after the \ */
            iochar = *((*pointerToInput)++);
            //checking for end of file
            if (iochar == '\0' ) {
                continue;
            }
            //checking for if we should should treat this as a pipe
            else if (iochar == '|') {
                isPipe = 0;
                //printf("not a pipe \n");
		(*pointerToInput)--;
                continue;
            }
            /*We should put the \n back if it's after a \ */
            else if (iochar == '\n') {
                (*pointerToInput)--;
                continue;
            }
            /* All other chars after a \ are printed normally */
        }
        /*Check for the end of a word */
        else if (iochar == space || iochar == '\n') {
            *letterLocation = '\0';
            /*Is this a leading space?*/
            if(n == 0 && iochar == space) {
                continue; 
            }
            /*Is this a newline after a leading space or file start?*/
            else if (n == 0 && iochar == '\n') { 
                return 0;
            }
            /*Is this a newline after a word?*/
            else if (n > 0 && iochar == '\n') {
                (*pointerToInput)--;
                return n;
            }
            /*This is the end of a word*/
            else {
                return n;
            }
        }
        /*Check for Special Character  */
        else if (iochar == '|' || iochar == '&' || iochar == '<' || iochar == '>') {
            
            // check for a pipe to set flag
            if (n == 0 && iochar == '|' && isPipe == 1) {
                //printf("setting pipe flag to one\n");
		        //printf("value of isPipe: %d\n" , isPipe);
		        pipeFlag = 1;
            }
            
            /*Is this the first char?*/
            if (n == 0 && iochar != '>') {
                *letterLocation++ = iochar;
                *letterLocation = '\0';
                return 1;
            }
            /*Is a this after a word?*/
            else if (n > 0) {
		        //if we have a pipe character that is not an actual pipe
                if (iochar == '|' && isPipe == 0) {
		            *letterLocation++ = iochar;
		    *letterLocation = '\0';
		    return n+1;
		}
		*letterLocation = '\0';
                /*Print the word first, then comback for this special char */
                (*pointerToInput)--;
                return n;
            }
            /*Is this a special case?*/
            else if (iochar == '>') {
                *letterLocation++ = iochar;
                if ((iochar = *((*pointerToInput)++)) == '\0') {
                *letterLocation = '\0';
                return 1;
                }
                /*Check for >& */
                else if (iochar == '&') {
                    *letterLocation++ = iochar;
                    *letterLocation = '\0';
                    return 2;
                }
                /*Check for >> */
                else if (iochar == '>') {
                *letterLocation++ = iochar;
                   /*Check for >>& */
                    if ((iochar = *((*pointerToInput)++)) == '\0') {
                        *letterLocation = '\0';
                        return 2;
                    }
                    else if (iochar == '&') {
                        *letterLocation++ = iochar;
                        *letterLocation = '\0';
                        return 3; 
                    }
                    /*This is just >> */
                    else {
                        /* we need the current random char for the next word */
                        (*pointerToInput)--;
                        *letterLocation = '\0';
                        return 2; 
                    }  
                }
                /*This is just > */
                else {
                    /*we need the current random char for the next word */
                    (*pointerToInput)--;
                    *letterLocation = '\0';
                    return 1;
                }
                
            }
        }
        /*Save the current char to the array*/
        *letterLocation++ = iochar;
        *letterLocation = '\0';
        /*Check for the word 'done' */
        if (n == lengthOfDone && *w == 'd' && *(++w) == 'o' && *(++w) == 'n' && *(++w) == 'e') {
            /* Ensure 'done' is not a prefix */
            if ((iochar = *((*pointerToInput)++)) == '\0' || iochar == space || iochar == '\n') {
                *letterLocation = '\0';  
                return -1;
            } else {
                /* we grabbed the next char after 'done' to check for end of word */
                (*pointerToInput)--;
            } 
        }
        n++;
        /*ensure there is space in Buffer*/
        if (n >= STORAGE-1) {
        return n;
        }
    }
    /*return -1 and empty string if EOF is encountered while n=0*/
    if (n == 0) { 
        *w = '\0';
    }
    /*return n if EOF is detected prematurely*/
    else if (n > 0) {
	return n;
    }
    return -1;
}
