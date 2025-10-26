/*
Participants: Donovan Dempster, Drew Ferrington
CSC 222: Shell Project
Due: 2/19/24
*/

// Imported libraries
#include <stdio.h> // Provide functions for standard input/output [printf(), fgets(), perror()]
#include <stdlib.h> // Provide functions for memory allocation, program termination, and other utilities [fgets(), malloc(), exit()]
#include <string.h> // Provide functions for string manipulation [strtok(), strcspn(), strdup()]

#ifdef _WIN32
#include <windows.h>
#include <io.h>
#include <fcntl.h>
#else
#include <unistd.h> // Provide functions for process management and system calls [fork(), execvp(), getcwd()]
#include <sys/wait.h> // Provide functions for process management [wait()]
#endif

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
        //Call CommandPrompt function to get the user's input
        input = CommandPrompt();
        // Parse user input
        command = ParseCommandLine(input);

        //Check if user wants to exit
        if(strcmp(command.args[0], "exit") == 0)
            {
                //Print end line and return 0 to end program
                printf("~$\n");
                return 0;
            }

        
        //Fork a child process. If in child process, execute the parsed command; if in parent, wait for child to finish
        ExecuteCommand(command);
    }
    return 0; //Exit program successfully 
}

//Display current working directory and return user input
char* CommandPrompt()
{
    char cwd[1024]; //Buffer to hold current working 
    #ifdef _WIN32
        if (_getcwd(cwd, sizeof(cwd)) == NULL)
    #else
        if(getcwd(cwd, sizeof(cwd))==NULL) //Get current working directory
    #endif
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
//Execute a shell command
void ExecuteCommand(struct ShellCommand command)
{
    // Handle cd
    if (strcmp(command.args[0], "cd") == 0){
        if (command.args[1] == NULL){
            perror("Directory name not provided");
            return;
        }
        if (chdir(command.args[1]) != 0){
            perror("chdir error");
        }
        return;
    }

    // Handle redirection
    int index = 0;
    while (command.args[index] != NULL &&
           strcmp(command.args[index], "<") != 0 &&
           strcmp(command.args[index], ">") != 0)
    {
        index++;
    }

    FILE* filePtr = NULL;
    if (command.args[index] != NULL)
    {
        if (command.args[index + 1] == NULL)
        {
            perror("File name not provided.");
            return;
        }

        if (strcmp(command.args[index], ">") == 0)
        {
            filePtr = fopen(command.args[index + 1], "w");
            #ifdef _WIN32
                _dup2(_fileno(filePtr), _fileno(stdout));
            #else
                dup2(fileno(filePtr), STDOUT_FILENO);
            #endif
        }
        else if (strcmp(command.args[index], "<") == 0)
        {
            filePtr = fopen(command.args[index + 1], "r");
            #ifdef _WIN32
                _dup2(_fileno(filePtr), _fileno(stdin));
            #else
                dup2(fileno(filePtr), STDIN_FILENO);
            #endif
        }

        if (filePtr) fclose(filePtr);

        // Remove redirection symbol and file name from arguments
        for (int i = index; command.args[i] != NULL; i++)
            command.args[i] = command.args[i + 2];
    }

    // Execute the command (cross-platform)
    #ifdef _WIN32
        // Build a cmd.exe command line
        char cmdLine[1024] = "cmd.exe /C ";
        for (int i = 0; command.args[i] != NULL; i++) {
            strcat(cmdLine, command.args[i]);
            strcat(cmdLine, " ");
        }

        STARTUPINFO si;
        PROCESS_INFORMATION pi;
        ZeroMemory(&si, sizeof(si));
        si.cb = sizeof(si);
        ZeroMemory(&pi, sizeof(pi));

        if (!CreateProcess(NULL, cmdLine, NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi)) {
            fprintf(stderr, "CreateProcess failed (%lu)\n", GetLastError());
            return;
        }

        WaitForSingleObject(pi.hProcess, INFINITE);
        CloseHandle(pi.hProcess);
        CloseHandle(pi.hThread);

    #else
        int pid = fork();
        if (pid == 0) {
            execvp(command.args[0], command.args);
            perror("execvp() error");
            exit(EXIT_FAILURE);
        } else if (pid > 0) {
            wait(NULL);
        } else {
            perror("fork error");
        }
    #endif
}
