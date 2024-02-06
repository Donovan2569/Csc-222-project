#include<stdio.h>
#include<unistd.h> 
#include<stdlib.h>
#include <sys/wait.h>

struct ShellCommand
{
    char *args[100];
};

char* CommandPrompt();

struct ShellCommand ParseCommandLine(char* input);

void ExecuteCommand(struct ShellCommand command);

int main()
{
    char* input;    
    struct ShellCommand command = {{"ls", NULL}};

    for(;;)
    {
        wait(NULL);
        input = CommandPrompt();

        //command = ParseCommandLine(input);

        int pid = fork();
        if (pid == 0)
            ExecuteCommand(command);
    }
    exit(0);
}

//Display current working directory and return user input
char* CommandPrompt()
{
    char input[100];
    int pid = fork();

    if (pid == 0)
        execlp("pwd", "pwd", NULL);
        

    else
    {
        fgets(input, 100, stdin);
        wait(NULL);
    }

    char* inputPtr = input;
    return inputPtr;
}

//Process the user input (As a shell command)
struct ShellCommand ParseCommandLine(char* input)
{ 
    
}

//Execute a shell command
void ExecuteCommand(struct ShellCommand command)
{
    execvp(command.args[0], command.args);
}