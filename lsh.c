#include "commands.c"
#include "lsh.h"

int main(int argc, char **argv)
{
    //Print banner
    lshBanner();
    //Initialize history
    historyInit(&hist);
    //Run command loop
    lshLoop();

    //Shutdown and Cleanup
    historyFree(&hist);
    return EXIT_SUCCESS;
}

//functions
void lshLoop(void)
{
    //declare variables
    char *line;
    char **args;
    int status;

    do
    {
        printCustomPrompt(); //prompt

        line = lshReadLine(); //call a function to read a line
        historyAdd(&hist, line); //add the line to history
        args = lshSplitLine(line); //split the line into arguments
        status = lshExecute(args); //execute the arguments

        //free memory
        free(line);
        free(args);
    } while(status);

}

char *lshReadLine(void)
{
    char *line = NULL;
    ssize_t bufsize = 0; //getline allocated a buffer

    if (getline(&line, &bufsize, stdin) == -1) //getline dynamically allocates memory if *line is NULL
    {
        if (feof(stdin)) 
        {
            exit(EXIT_SUCCESS);  //recieved an EOF(Ctrl+D)
        } else  {
            perror("readline");
            exit(EXIT_FAILURE);
        }
    }

    return line;

}

char **lshSplitLine(char *line) 
{ 
    int bufferSize = LSH_TOK_BUFSIZE, position = 0; //initial capacity for tokens array and current index
    char **tokens = malloc(bufferSize * sizeof(*tokens)); //each entry will point to a token string

    if(!tokens) //if malloc fails, print the error
    {
        fprintf(stderr, "lsh: Allocation Error!\n");
        exit(EXIT_FAILURE);
    }

    char *current = malloc(strlen(line) + 1); //temporary buffer to hold one token at a time
    int curPos = 0; //current position
    bool inQuote = false;
    char quoteChar = '\0'; //remembers which type of qoute (' or ")

    for(int i = 0; line[i] != '\0'; i++) //iterate through every character in line
    {
        char c = line[i];

        if(inQuote)
        {
            if(c == quoteChar) //if in a qoute and 'c' matches (' or ")
            {
                current[curPos] = '\0'; //null terminate the token
                tokens[position++] = strdup(current); // makes a copy with strdup
                curPos = 0;
                inQuote = false;
            }
            else // we keep adding character to current
            {
                current[curPos++] = c;
            }
        }
        else // if not inside a qoute
        {
            if(c == '"' || c == '\'')  //if 'c' is ['] or ["], it is qouted
            {
                inQuote = true;
                quoteChar = c;
            }
            else if (c == ' ' || c == '\t' || c == '\n') // partial token will be finished and stored
            {
                if(curPos > 0)
                {
                    current[curPos] = '\0';
                    tokens[position++] = strdup(current);
                    curPos = 0;
                }
            }
            else if (c == '|' || c == '<' || c == '>') // store operator as its own token
            {
                if(curPos > 0)
                {
                    current[curPos] = '\0';
                    tokens[position++] = strdup(current);
                    curPos = 0;
                }
                tokens[position++] = strndup(&line[i], 1); //duplicate just one character into a new string
            }
            else
            {
                current[curPos++] = c; //append to current if normal character
            }
        }

        if(position >= bufferSize) //if next position exceeds bufferSize, reallocate
        {
            bufferSize *= 2;
            char **temp = realloc(tokens, bufferSize * sizeof(*tokens)); 

            if(!temp) //if realloc fails, tokens will still point to the old block
            {
                free(tokens);
                fprintf(stderr, "lsh: Allocation Error!\n");
                exit(EXIT_FAILURE);
            }
            
            tokens = temp;
        
        }

    }

    if(curPos > 0) // finish building the token
    {
        current[curPos] = '\0';
        tokens[position++] = strdup(current);
    }

    tokens[position] = NULL; //add the final NULL to the array
    free(current);

    return tokens; //return the array of tokens
}

int lshLaunch(char **args)
{
    pid_t pid, wpid; //pid stores result of fork(), wpid used when waiting for the child process
    int status;

    pid = fork(); //create a new process

    if(pid == 0) //child process will return 0
    { 
        //child replace itself with the requested program  
        if(execvp(args[0], args) == -1) //if execvp returns -1, an error occurred, execvp searches the system PATH
        {
            perror("lsh");
        }
        exit(EXIT_FAILURE);
    }
    else if(pid < 0)
    {
        perror("lsh"); //error forking
    }
    else
    { //parent process
        do
        {
            wpid = waitpid(pid, &status, WUNTRACED); //wait for child process with pid and stores child's exit into into status

        } while(!WIFEXITED(status) && !WIFSIGNALED(status));//keep waiting if the child process has not exited or been killed
    }

    return 1;

}

int lshExecute(char **args)
{
    int i;

    if(args[0] == NULL)
    {
        return 1; //return 1 if an empty command is entered
    }

    for(i = 0; i < lshNumBuiltIns(); i++)//check if the command matches any built-in commands
    {
        if(strcmp(args[0], builtInStr[i]) == 0)
        {
            return(*builtInFunction[i]) (args); //call the corresponding function if match is found
        }
    }

    return lshLaunch(args); //if no match is found, launch it as a program
}

//misc built-ins
void printCustomPrompt()
{
    char cwd[PATH_MAX]; //buffer for current working directory
    char host[HOST_NAME_MAX]; //buffer for hostname
    char *home = getenv("HOME"); //store hostname
    char *user = getenv("USER");//store user

    if(getcwd(cwd, sizeof(cwd)) == NULL) //print error if getcwd fails
    {
        perror("getcwd");
        return;
    }

    if(gethostname(host, sizeof(host)) != 0) //print error if gethostname fails
    {
        perror("gethostname");
        return;
    }

    const char *shortPath = cwd; 
    if (home && strncmp(cwd, home, strlen(home)) == 0) //if cwd starts with "home"
    {
        shortPath = cwd + strlen(home); //skip over "home" part
        printf(COLOR_RED "%s" COLOR_GREEN "@%s" COLOR_YELLOW ":" COLOR_BLUE "%s" COLOR_RESET ">> ", user, host, shortPath); //print prompt with '~' for home directory
    } else 
    {
        printf(COLOR_RED "%s" COLOR_GREEN "@%s" COLOR_YELLOW ":" COLOR_BLUE "%s" COLOR_RESET ">> " , user, host, cwd); //print full path if not in home directory
    }

}

void historyInit(History *h)
{
    h->entries = calloc(HISTORY_CAPACITY, sizeof(char*)); //allocate memory for history entries
    h->capacity = HISTORY_CAPACITY;
    h->size = 0;
    h->start = 0;
    h->total = 0;
}

int isAllWhiteSpace(const char *s)
{
    while(*s)
    {
        if(!isspace((unsigned char) *s)) return 0; //return if a non-whitespace character is found
        s++;
    }
    return 1;
}

void historyAdd(History *h, const char *line)
{
    if(!line || isAllWhiteSpace(line)) //ignore empty or whitespace-only commands
    {
        return;
    }

    char* temp = strdup(line); //duplicate the command string
    if (!temp) 
    {
        fprintf(stderr, "lsh: history allocation error\n");
        return;
    }

    if(h->size < h->capacity) //there is still space in history
    {
        h->entries[(h->start + h->size) % h->capacity] = temp; //add new entry at the end
        h->size++; //increase size
    }
    else
    {
        free(h->entries[h->start]); //free the oldest entry
        h->entries[h->start] = temp; //replace it with the new entry
        h->start = (h->start + 1) % h->capacity;//move start to the next oldest entry
    }
    h->total++; //increment total commands added
}

void historyPrint(const History *h)
{
    unsigned long firstNum = h->total - h->size + 1; //the number of the first command in history

    for(size_t i = 0; i < h->size; i++) //print each entry in order
    {
        size_t index = (h->start + i) % h->capacity; //calculate the correct index in the circular buffer
        printf(COLOR_GREEN"%lu" COLOR_YELLOW":" COLOR_RESET"%s\n", firstNum + i, h->entries[index]); //print command number and entry
    }
}

void historyFree(History *h)
{
    for(size_t i = 0; i < h->size; i++) 
    {
        free(h->entries[(h->start + i) % h->capacity]); //free each entry
    }
    free(h->entries); //free the entries array
}
