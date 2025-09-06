#include "main.h"

#define LSH_RL_BUFSIZE 1024 //1kb of buffer size
#define LSH_TOK_BUFSIZE 62 //token size of 64bytes
#define LSH_TOK_DELIM " \t\r\n\a" //delimiters for tokenizing, passed into strtok to tell which separate tokens

//Function Declarations
void lshLoop(void);
char *lshReadLine(void);
char **lshSplitLine(char *line);


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
        //line = lshReadLine(); //call a function to read a line
        //args = lshSplitLine(line); //split the line into arguments
        //status - lshExecute(args); //execute the arguments

        //free memory
        free(line);
        free(args);
    } while(status);

}

char *lshReadLine(void)
{
    //declare variables
    int bufferSize = LSH_RL_BUFSIZE;
    int position = 0;
    char *buffer = malloc(sizeof(char) * bufferSize); //allocate memory for the buffer
    int c;

    if(!buffer)
    {
        fprintf(stderr, "lsh: Allocation Error!\n"); //buffer allocation error, program terminates
        exit(EXIT_FAILURE);
    }

    while(1)
    {
        c = getchar(); //read a character and store it as an integer

        if(c == EOF || c == '\n') //if we reach the end of the file(or Ctrl+D) or a new line, then we
        {
            buffer[position] = '\0'; //replace with null and return
            return buffer;
        }
        else
        {
            buffer[position] = c; //else we add character to buffer
        }
        position++;
    }

    if(position >= bufferSize) // if the next char exceeds the current buffer size, then reallocate with +1kb
    {
        bufferSize += LSH_RL_BUFSIZE;
        buffer = realloc(buffer, bufferSize);
        
        if(!buffer)
        {
            fprintf(stderr, "lsh: Allocation Error!\n"); //exit if allocation encountered an error
            exit(EXIT_FAILURE);
        }
    }
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
            char **temp = realloc(tokens, bufferSize * sizeof(char*)); 

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






