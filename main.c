#include "main.h"

#define LSH_RL_BUFSIZE 1024 //1kb of buffer size

void lsh_loop(void);

int main(int argc, char **argv)
{
    //Load Config Files


    //Run command loop
    lsh_loop();

    //Shutdown and Cleanup
    return EXIT_SUCCESS;
}

void lsh_loop(void)
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
        status - lshExecute(args); //execute the arguments

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





