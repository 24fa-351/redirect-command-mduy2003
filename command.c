#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

#include "command.h"

void split_command(const char *command, char ***args, int *arg_count)
{
    *arg_count = 0;
    char *command_copy = strdup(command);
    char *token = strtok(command_copy, " "); // Split command by spaces
    *args = malloc(sizeof(char *) * MAX_ARGS);
    while (token != NULL && *arg_count < MAX_ARGS) // Adds each separated command to args
    {
        (*args)[*arg_count] = strdup(token);
        (*arg_count)++;
        token = strtok(NULL, " ");
    }
    (*args)[*arg_count] = NULL;
    free(command_copy);
}

char *find_absolute_path(char *command)
{
    char *path_environment = getenv("PATH"); // Get the PATH environment variable
    if (!path_environment)
    {
        return NULL;
    }

    char *path_copy = strdup(path_environment);
    char *directory = strtok(path_copy, ":"); // Split using colons because PATH is colon-separated

    while (directory != NULL)
    {
        char path[MAX_PATH_LENGTH];
        snprintf(path, sizeof(path), "%s/%s", directory, command); // Concatenate directory and command

        if (access(path, X_OK) == 0) // Check if the command exists
        {
            free(path_copy);
            return strdup(path);
        }
        directory = strtok(NULL, ":");
    }
}
