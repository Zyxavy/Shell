#include "main.h"
#include "commands.c"

int main(int argc, char **argv)
{
    //Load Config Files


    //Run command loop
    lshLoop();

    //Shutdown and Cleanup
    return EXIT_SUCCESS;
}

void lshLoop(void)
{
    //declare variables
    char *line;
    char **args;
    int status;

    do
    {
        printf(">> "); //prompt
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

char **lshSplitLine(char *line) //returns a pointer to an array of char* pointers(an array of strings)
{
    int bufferSize = LSH_TOK_BUFSIZE, position = 0; 
    char **tokens = malloc(bufferSize * sizeof(*tokens));//allocate memory for tokens array
    char *token, *saveptr; //temporary variable to hold each token

    if(!tokens)
    {
        fprintf(stderr, "lsh: Allocation Error!\n");
        exit(EXIT_FAILURE);
    }

    token = strtok_r(line, LSH_TOK_DELIM, &saveptr); //scans the line and returns the first sequence of char that does not contain the delimeters
    while(token != NULL) //while there are tokens
    {
        tokens[position] = token; //store token to the array
        position++;

        if(position >= bufferSize) //if next position exceeds bufferSize, reallocate
        {
            bufferSize += LSH_TOK_BUFSIZE;
            char **temp = realloc(tokens, bufferSize * sizeof(*tokens)); 

            if(!temp) //if realloc fails, tokens will still point to the old block
            {
                free(tokens);
                fprintf(stderr, "lsh: Allocation Error!\n");
                exit(EXIT_FAILURE);
            }
            
            tokens = temp;
        
        }

        token = strtok_r(NULL, LSH_TOK_DELIM, &saveptr); //get next token
    }

    tokens[position] = NULL; //terminate the array with NULL
    return tokens;
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

