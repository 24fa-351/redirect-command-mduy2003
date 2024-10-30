#ifndef REDIR_H
#define REDIR_H

#define MAX_ARGS 10
#define MAX_PATH_LENGTH 512

void split_command(const char *command, char ***args, int *arg_count);

char *find_absolute_path(char *command);

#endif