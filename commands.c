#include "lsh.h"



//list of commands
char *builtInStr[] = {
    "cd", "help", "exit", "pwd", "echo", "clear", "history", "mkdir"
};

//their corresponding functions
int (*builtInFunction[]) (char**) = {
    &lshCd, &lshHelp, &lshExit, &lshPwd, &lshEcho,
    &lshClear, &lshHistory, &lshMkdir
};

int lshNumBuiltIns() //returns the number of built-in commands
{
    return sizeof(builtInStr) / sizeof(char*);
}

void lshBanner() 
{
    printf(
    " __        ______  ________  ________  __        ________               ______   __    __  ________  __        __       \n"
    "/  |      /      |/        |/        |/  |      /        |             /      \\ /  |  /  |/        |/  |      /  |      \n"
    "$$ |      $$$$$$/ $$$$$$$$/ $$$$$$$$/ $$ |      $$$$$$$$/             /$$$$$$  |$$ |  $$ |$$$$$$$$/ $$ |      $$ |      \n"
    "$$ |        $$ |     $$ |      $$ |   $$ |      $$ |__                $$ \\__$$/ $$ |__$$ |$$ |__    $$ |      $$ |      \n"
    "$$ |        $$ |     $$ |      $$ |   $$ |      $$    |               $$      \\ $$    $$ |$$    |   $$ |      $$ |      \n"
    "$$ |        $$ |     $$ |      $$ |   $$ |      $$$$$/                 $$$$$$  |$$$$$$$$ |$$$$$/    $$ |      $$ |      \n"
    "$$ |_____  _$$ |_    $$ |      $$ |   $$ |_____ $$ |_____             /  \\__$$ |$$ |  $$ |$$ |_____ $$ |_____ $$ |_____ \n"
    "$$       |/ $$   |   $$ |      $$ |   $$       |$$       |            $$    $$/ $$ |  $$ |$$       |$$       |$$       |\n"
    "$$$$$$$$/ $$$$$$/    $$/       $$/    $$$$$$$$/ $$$$$$$$/              $$$$$$/  $$/   $$/ $$$$$$$$/ $$$$$$$$/ $$$$$$$$/ \n"
    "\n"
        );
}

int lshCd(char **args)
{
    if (args[1] == NULL) //if no argument is given to cd
    {
        fprintf(stderr, "lsh: expected argument to \"cd\"\n");
    } 
    else 
    {
        if (chdir(args[1]) != 0) //change directory, if it returns -1 then an error occurred
        {
            perror("lsh");
        }
    }
    return 1;
}

int lshHelp(char **args)
{
    int i;
    lshBanner();
    printf("Type program names and arguments, and hit enter.\n");
    printf("The following are built in:\n");

    for(i = 0; i < lshNumBuiltIns(); i++) //list all built-in commands
    {
        printf("  %s\n", builtInStr[i]); //print each command
    }

    return 1;
}

int lshExit(char **args)
{
    return 0;
}

int lshPwd(char **args)
{
    char buffer[PATH_MAX]; // allocate a buffer of PATH_MAX

    if(getcwd(buffer, PATH_MAX) == NULL) //call getcwd and if fail we print the error
    {
        perror("pwd"); 
    }
    else
    {
        printf("%s\n", buffer); //print the current working directory
    }

    return 1; // keep shell running
}

int lshEcho(char **args)
{
    int i = 1; //start at 1 so we ignore "echo"

    while(args[i] != NULL) //if it is not Null we print the arguments and stop at null
    {
        printf("%s", args[i]);

        if(args[i + 1] != NULL)
        {
            printf(" ");
        }
        i++;
    }
    printf("\n");
    
    return 1; // keep the shell running
}

int lshClear(char **args)
{
    printf("\033[H\033[J");//clears the terminal "\033 escape char" "[H moves the cursor the the 'home' position" "[J clears the screen"
    return 1;
}

int lshHistory(char **args)
{
    (void)*args; //suppresses unused parameter warning
    historyPrint(&hist); //print the command history
    return 1;
}

int lshMkdir(char **args)
{
    if(args[1] == NULL) // if there is no argument for mkdir print an error
    {
        printf("mkdir: missing operand");
    }
    else
    {
        int result = mkdir(args[1], 0755); //mkdir is already provided by the OS, this function is basically just a wrapper
        if(result == -1)
        {
            perror("mkdir");
        }
    }

    return 1;
}