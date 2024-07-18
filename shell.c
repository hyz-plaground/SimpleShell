#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>

int main(int argc, const char *argv[])
{
    printf("Welcome to my shell!\n");

    for (;;)
    {
        printf("# ");

        // Read & Process user input
        char *line = NULL;
        size_t size;
        size_t line_len = getline(&line, &size, stdin);
        if (line_len <= 0)
        {
            continue;
        }
        *(line + line_len - 1) = '\0'; // Remove line break char

        // Remaining args for excvp
        char *args[2];
        args[0] = line;
        args[1] = NULL;

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
            free(line); // Free up heap space for getline
        }
    }
    return 0;
}