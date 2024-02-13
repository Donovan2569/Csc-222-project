/*
Participants: Donovan Dempster, Drew Ferrington
CSC 222: Shell Project
Due: 2/19/24
*/

// Imported libraries
#include <stdio.h> // Provide functions for standard input/output [printf(), fgets(), perror()]
#include <unistd.h> // Provide functions for process management and system calls [fork(), execvp(), getcwd()]
#include <stdlib.h> // Provide functions for memory allocation, program termination, and other utilities [fgets(), malloc(), exit()]
#include <string.h> // Provide functions for string manipulation [strtok(), strcspn(), strdup()]
#include <sys/wait.h> // Provide functions for process management [wait()]

// Max number of command line arguments
#define MAX_ARGS 100

//Holds parsed shell command and its arguments
struct ShellCommand
{
    char *args[MAX_ARGS];
};

//(FUNCTION PROTOTYPES)

//Display current working directory and return user input
char* CommandPrompt();

//Process the user input (As a shell command); Note the 
//return type is a ShellCommand struct
struct ShellCommand ParseCommandLine(char* input);

//Execute a shell command; Note the parameter
//is a ShellCommand struct
void ExecuteCommand(struct ShellCommand command);

//(END OF PROTOTYPES)

int main()
{
    char* input; // Pointer holds user input
    struct ShellCommand command; // Holds parsed command

    // Infinitely prompt the user for shell input
    for(;;)
    {
        wait(NULL);
        //Call CommandPrompt function to get the user's input
        input = CommandPrompt();
        // Parse user input
        command = ParseCommandLine(input);

        //Fork a child process. If in child process, execute the parsed command; if in parent, wait for child to finish
        int pid = fork();
        if (pid == 0)
            ExecuteCommand(command);
        else
            wait(NULL);
    }
    exit(0); //Exit program successfully 
}

//Display current working directory and return user input
char* CommandPrompt()
{
    char cwd[1024]; //Buffer to hold current working directory
    if(getcwd(cwd, sizeof(cwd))==NULL) //Get current working directory
    {
        perror("getcwd() error"); //Handle error if getcwd() failes
        exit(EXIT_FAILURE);
    }
    printf("%s$ ",cwd); //Display current working directory
    fflush(stdout); //Flush output buffer to ensure prompt is displayed
    char* input=malloc(sizeof(char)*100); //Allocate memory for user input
    if(input==NULL) //Handle error if malloc() failes
    {
        perror("malloc() error");
        exit(EXIT_FAILURE);
    }
    fgets(input,100,stdin); //Get user input from stdin
    input[strcspn(input, "\n")]=0; //Remove trailing newline character

    return input;//Return user input
}

//Process the user input (As a shell command and its arguments)
struct ShellCommand ParseCommandLine(char* input)
{ 
    struct ShellCommand command; //Structure to hold parsed command
    char* token; //Pointer to hold tokenized strings
    int i=0; //Index variable
    
    token=strtok(input," "); //Tokenize input using space delimeter
    while (token != NULL && i < MAX_ARGS-1) //Iterate through tokens
    {
        command.args[i]=strdup(token); //Store tokens in args array
        token=strtok(NULL," "); //Get next token
        i++; //Increment index
    }
    command.args[i]=NULL;//Set last argument to NULL
    return command;//Return parsed command
}

//Execute a shell command
void ExecuteCommand(struct ShellCommand command)
{
    execvp(command.args[0], command.args);
}
