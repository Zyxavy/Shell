#ifndef LSH_H
#define LSH_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>
#include <unistd.h> //for fork() and etc.
#include <sys/wait.h> // for waitpid()
#include "limits.h" // for PATH_MAX
#include <bits/local_lim.h> //for HOST_NAME_MAX

//Macros
#define LSH_RL_BUFSIZE 1024 //1kb of buffer size
#define LSH_TOK_BUFSIZE 62 //token size of 64bytes
#define LSH_TOK_DELIM " \t\r\n\a" //delimiters for tokenizing, passed into strtok to tell which separate tokens
#define HISTORY_CAPACITY 500 //max number of commands to store in history
#define COLOR_RESET "\033[0m" //ANSI escape code to reset color
#define COLOR_GREEN "\033[1;32m" //ANSI escape code for green text
#define COLOR_BLUE "\033[1;34m" //ANSI escape code for blue text
#define COLOR_RED "\033[1;31m" //ANSI escape code for red text
#define COLOR_YELLOW "\033[1;33m" //ANSI escape code for yellow text

//Data structures
typedef struct { //way to store History
    char **entries;
    size_t capacity;
    size_t size;
    size_t start;
    unsigned long total;
} History;

History hist; //global history variable


//Function Declarations
void lshLoop(void);
char *lshReadLine(void);
char **lshSplitLine(char *line);
int lshExecute(char **args);

//misc built-ins
void printCustomPrompt();
void historyInit(History *h);
int isAllWhiteSpace(const char *s);
void historyAdd(History *h, const char *line);
void historyPrint(const History *h);
void historyFree(History *h);

//commands
int lshCd(char **args);
int lshHelp(char **args);
int lshExit(char **args);
int lshPwd(char **args);
int lshEcho(char **args);
int lshClear(char **args);
int lshHistory(char **args);
void lshBanner(); 



#endif // LSH_H