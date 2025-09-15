#include "main.h"
#include "commands.c"

int main(int argc, char **argv)
{
    //print banner
    lshBanner();
    //Run command loop
    lshLoop();

    //Shutdown and Cleanup
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
    char cwd[PATH_MAX];
    char host[HOST_NAME_MAX];
    char *home = getenv("HOME");
    char *user = getenv("USER");

    if(getcwd(cwd, sizeof(cwd)) == NULL)
    {
        perror("getcwd");
        return;
    }

    if(gethostname(host, sizeof(host)) != 0)
    {
        perror("gethostname");
        return;
    }

    const char *shortPath = cwd;
    if (home && strncmp(cwd, home, strlen(home)) == 0) 
    {
        shortPath = cwd + strlen(home);
        printf("%s@%s:~%s$ ", user, host, shortPath);
    } else 
    {
        printf("%s@%s:%s$ ", user, host, cwd);
    }


}