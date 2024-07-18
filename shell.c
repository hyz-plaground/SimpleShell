#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>

char **read_user_input();

int main(int argc, const char *argv[])
{
    printf("Welcome to my shell!\n");

    for (;;)
    {
        printf("# ");

        char **args = read_user_input();

        if (args == NULL)
        {
            continue;
        }

        // Run command in a seperate process
        int ret_val = fork();
        if (ret_val < 0)
        {
            fprintf(stderr, "Fork Failed.\n");
        }
        else if (ret_val == 0)
        {
            // Child process
            int ret_code = execvp(args[0], args); // Execute given command.
            if (ret_code == -1)
            {
                fprintf(stderr, "Command not found.\n");
            }
        }
        else
        {
            // Parent process
            int wstatus;
            int terminated_pid = wait(&wstatus);
            if (terminated_pid == -1)
            {
                fprintf(stderr, "Wait Failed.\n");
            }

            // Free the allocated memory for arguments
            for (int i = 0; args[i] != NULL; ++i)
            {
                free(args[i]);
            }
            free(args);
        }
    }
    return 0;
}

/**
 * @brief Read user input.
 *
 * @return char**
 */
char **read_user_input()
{
    char *line = NULL;
    size_t size;
    size_t line_len = getline(&line, &size, stdin);
    if (line_len <= 0)
    {
        free(line);
        return NULL;
    }
    *(line + line_len - 1) = '\0'; // Remove break

    // Split input string into tokens based on \s
    char *token = strtok(line, "\t\n\r\f\v");
    int arg_count = 0;
    while (token != NULL)
    {
        arg_count++;
        token = strtok(NULL, "\t\n\r\f\v");
    }

    // Allocate mem for arg array
    char **args = malloc((arg_count + 1) * sizeof(char *));
    if (args == NULL)
    {
        free(line);
        return NULL;
    }

    token = strtok(line, "\t\n\r\f\v");
    int i = 0;
    while (token != NULL)
    {
        args[i] = strdup(token);
        if (args[i] == NULL)
        {
            for (int j = 0; j < i; ++j)
            {
                free(args[j]);
            }
            free(args);
            free(line);
            return NULL;
        }
        i++;
        token = strtok(NULL, "\t\n\r\f\v");
    }
    args[i] = NULL;
    free(line);
    return args;
}