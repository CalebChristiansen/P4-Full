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


int getword(char *w, char *input) {
    int iochar, n = 0;
    char *letterLocation = w; 

    while ( ( iochar = *(input++) ) != '\0' ) {
        /*Check for ignoring special characters */
        if (iochar == '\\') {
            /*Grab the char after the \ while checking for end of file */
            if ((iochar = *(input++)) == '\0' ) {
                continue; 
            }
            /*We should put the \n back if it's after a \ */
            else if (iochar == '\n') {
                input--;
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
                input--;
                return n;
            }
            /*This is the end of a word*/
            else {
                return n;
            }
        }
        /*Check for Special Character  */
        else if (iochar == '|' || iochar == '#' || iochar == '&' || iochar == '<' || iochar == '>') {
            /*Is this the first char?*/ 
            if (n == 0 && iochar != '>') {
                *letterLocation++ = iochar;
                *letterLocation = '\0';
                return 1;
            }
            /*Is a this after a word?*/
            else if (n > 0) {
                letterLocation = '\0';
                /*Print the word first, then comback for this special char */
                input--;
                return n;
            }
            /*Is this a special case?*/
            else if (iochar == '>') {
                *letterLocation++ = iochar;
                if ((iochar = *(input++)) == '\0') {
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
                    if ((iochar = *(input++)) == '\0') {
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
                        input--;
                        *letterLocation = '\0';
                        return 2; 
                    }  
                }
                /*This is just > */
                else {
                /*we need the current random char for the next word */
                input--;
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
            if ((iochar = *(input++)) == '\0' || iochar == space || iochar == '\n') {
                *letterLocation = '\0';  
                return -1;
            } else {
                /* we grabbed the next char after 'done' to check for end of word */
                input--;
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
