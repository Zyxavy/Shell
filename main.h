#ifndef MAIN_H
#define MAIN_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h> //for fork() and etc.
#include <sys/wait.h> // for waitpid()


#define LSH_RL_BUFSIZE 1024 //1kb of buffer size
#define LSH_TOK_BUFSIZE 62 //token size of 64bytes
#define LSH_TOK_DELIM " \t\r\n\a" //delimiters for tokenizing, passed into strtok to tell which separate tokens


//Function Declarations
void lshLoop(void);
char *lshReadLine(void);
char **lshSplitLine(char *line);
int lshExecute(char **args);
int lshCd(char **args);
int lshHelp(char **args);
int lshExit(char **args);



#endif // MAIN_H