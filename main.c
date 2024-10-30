#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <sys/wait.h>

#include "command.h"

extern char **environ; // Global environment variables

int main(int argc, char *argv[])
{
    if (argc != 4)
    {
        fprintf(stderr, "Usage: ./redir <input> <command> <output>\n");
        return 1;
    }

    char *input_file = argv[1];
    char *command = argv[2];
    char *output = argv[3];
    char **args;
    int arg_count;

    split_command(command, &args, &arg_count);
    if (arg_count == 0)
    {
        fprintf(stderr, "No command provided\n");
        return 1;
    }

    char *command_path = args[0];
    if (command_path[0] != '/') // Checks if the command is an absolute path
    {
        char *absolute_path = find_absolute_path(args[0]);
        if (!absolute_path)
        {
            fprintf(stderr, "Command not found: %s\n", args[0]);
            return 1;
        }
        command_path = absolute_path;
    }

    pid_t pid = fork();
    if (pid == -1)
    {
        perror("fork");
        return 1;
    }

    if (pid == 0) // Child Process
    {
        if (strcmp(input_file, "-") != 0) // Checks if input file is not "-"
        {
            int input_fd = open(input_file, O_RDONLY);
            if (input_fd == -1)
            {
                perror("Couldn't open input file");
                exit(1);
            }
            dup2(input_fd, STDIN_FILENO);
            close(input_fd);
        }

        int output_fd = STDOUT_FILENO;
        if (strcmp(output, "-") != 0) // Checks if output file is not "-"
        {
            output_fd = open(output, O_WRONLY | O_CREAT | O_TRUNC, 0666); //
            if (output_fd == -1)
            {
                perror("Couldn't open output file");
                exit(1);
            }
            dprintf(output_fd, "Command: %s\n", command);
        }
        dup2(output_fd, STDOUT_FILENO);
        if (output_fd != STDOUT_FILENO)
        {
            close(output_fd);
        }
        execve(command_path, args, environ);
        perror("execve");
        exit(1);
    }
    else // Parent Process
    {
        int status;
        waitpid(pid, &status, 0);
    }

    for (int i = 0; i < arg_count; i++)
    {
        free(args[i]);
    }
    free(args);

    if (command_path != args[0])
    {
        free(command_path);
    }

    return 0;
}