#ifndef MAIN_H
#define MAIN_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h> //for fork() and etc.
#include <sys/wait.h> // for waitpid()
#include "limits.h" // for PATH_MAX


#define LSH_RL_BUFSIZE 1024 //1kb of buffer size
#define LSH_TOK_BUFSIZE 62 //token size of 64bytes
#define LSH_TOK_DELIM " \t\r\n\a" //delimiters for tokenizing, passed into strtok to tell which separate tokens
#define COLOR_RESET "\033[0m" //ANSI escape code to reset color
#define COLOR_GREEN "\033[1;32m" //ANSI escape code for green text
#define COLOR_BLUE "\033[1;34m" //ANSI escape code for blue text
#define COLOR_RED "\033[1;31m" //ANSI escape code for red text
#define COLOR_YELLOW "\033[1;33m" //ANSI escape code for yellow text


//Function Declarations
void lshLoop(void);
char *lshReadLine(void);
char **lshSplitLine(char *line);
int lshExecute(char **args);
void lshBanner(); 

//misc built-ins
void printCustomPrompt();

//commands
int lshCd(char **args);
int lshHelp(char **args);
int lshExit(char **args);
int lshPwd(char **args);
int lshEcho(char **args);
int lshClear(char **args);


#endif // MAIN_H