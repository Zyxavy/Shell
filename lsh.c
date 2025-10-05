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

int lshLaunch(char **args, bool background)
{
    //Fork and launch the process
    pid_t pid, wpid;
    int status;

    pid = fork(); //create a new process

    if (pid == 0)
    {
        // Child process
        if (execvp(args[0], args) == -1)  
        {
            perror("lsh");
        }
        exit(EXIT_FAILURE); // execvp only returns on error
    } 
    else if (pid < 0) //if it returns -1 it means there was an error
    {
        // Error forking
        perror("lsh");
    } 
    else 
    {
        // Parent process
        if (background)
        {
            printf("[pid %d] running in the background\n", pid);
        } 
        else 
        {
            do
            {
                wpid = waitpid(pid, &status, WUNTRACED); //wait for child process to change state
            } while (!WIFEXITED(status) && !WIFSIGNALED(status)); //keep waiting if the child hasn't exited or been killed
        }
    }

    return 1; //return 1 to continue the shell loop
}

int lshExecute(char **args)
{
    if (args[0] == NULL) 
    {
        return 1; // An empty command was entered.
    }

    int numPipes = 0; //count number of pipes in args
    for(int i = 0; args[i]; i++)
    {
        if(strcmp(args[i], "|") == 0) numPipes++; //increment for each pipe found
    }

    if(numPipes > 0)
    {
        int numCmds = numPipes + 1; //number of commands is one more than number of pipes
        char ***cmds = malloc(numCmds * sizeof(char***));//array of command arrays
        int start = 0, cmdIndex = 0;

        for(int i = 0; ; i++)//iterate through the args
        {
            if(args[i] == NULL || strcmp(args[i], "|") == 0) //if end of args or pipe is found
            {
                int len = i - start;
                cmds[cmdIndex] = malloc((len + 1) * sizeof(char*)); //allocate space for this command

                for(int j = 0; j < len; j++)
                {
                    cmds[cmdIndex][j] = args[start + j]; //copy the argument into the command array
                }

                cmds[cmdIndex][len] = NULL; //null terminate the command array

                cmdIndex++;
                start = i + 1;

                if(args[i] == NULL) break;
            }
        }

        int status = lshExecutePiped(cmds, numCmds); //execute the piped commands

        for(int i = 0; i < numCmds; i++) //free each command array
        {
            free(cmds[i]);
        }
        free(cmds);

        return status;
    }




    //File descriptors for redirection all are in invalid states and will be changed once they are used
    int saved_stdin = -1; 
    int saved_stdout = -1; 
    int in_fd = -1;
    int out_fd = -1; 
    char *inputFile = NULL;
    char *outputFile = NULL;

    //Scan for redirections and setup file descriptors
    for (int i = 0; args[i] != NULL; i++) 
    {
        
        if (strcmp(args[i], "<") == 0) //input redirection
        {
            if (args[i+1] == NULL) //no file specified after '<'
            {
                fprintf(stderr, "lsh: syntax error near unexpected token `<'\n");
                return 1;
            }

            inputFile = args[i+1];
            args[i] = NULL; //Remove redirection operator and filename from args
            args[i+1] = NULL;
            i++; //Skip the filename in the next iteration
        } 
        else if (strcmp(args[i], ">") == 0) //output redirection
        {
            if (args[i+1] == NULL) {
                fprintf(stderr, "lsh: syntax error near unexpected token `>'\n");
                return 1;
            }

            outputFile = args[i+1];
            args[i] = NULL; //Remove redirection operator and filename from args
            args[i+1] = NULL;
            i++; //Skip the filename in the next iteration
        }
    }

    //Apply the redirections 
    if (inputFile) 
    {
        in_fd = open(inputFile, O_RDONLY); //Open file for reading

        if (in_fd < 0) //if an error occurs when opening file
        {
            perror("lsh: open");
            return 1;
        }

        saved_stdin = dup(STDIN_FILENO); //Save original stdin
        dup2(in_fd, STDIN_FILENO); //Redirect stdin
    }
    if (outputFile) 
    {
        out_fd = open(outputFile, O_WRONLY | O_CREAT | O_TRUNC, 0644); //Open file for writing, create if it doesn't exist, truncate if it does
        if (out_fd < 0) //if an error occurs when opening file
        {
            perror("lsh: open");
            return 1;
        }

        saved_stdout = dup(STDOUT_FILENO); //Save original stdout
        dup2(out_fd, STDOUT_FILENO); //Redirect stdout
    }

    //Execute the command or builtins
    int status = 1;
    bool is_builtin = false;
    for (int i = 0; i < lshNumBuiltIns(); i++) 
    {
        if (strcmp(args[0], builtInStr[i]) == 0) //check if the command matches a built-in
        {
            status = (*builtInFunction[i])(args); //call the built-in function
            is_builtin = true;
            break;
        }
    }

    if (!is_builtin) //not a built-in command
    {
        //Handle background processes for external commands
        bool background = false;
        int last = 0;

        while(args[last] != NULL) last++; //find the last argument

        if (last > 0 && strcmp(args[last-1], "&") == 0) //if last argument is '&' it becomes a background process
        {
            background = true;
            args[last-1] = NULL;
        }

        status = lshLaunch(args, background); //launch the external command
    }
    
    //Restore stdin and stdout & close files
    if (saved_stdin != -1) 
    {
        dup2(saved_stdin, STDIN_FILENO);
        close(saved_stdin);
        close(in_fd);
    }
    if (saved_stdout != -1) 
    {
        dup2(saved_stdout, STDOUT_FILENO); 
        close(saved_stdout);
        close(out_fd);
    }

    return status;
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



