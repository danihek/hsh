#include "bits/types/stack_t.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <limits.h>
#include <sys/wait.h>


char *hsh_read_line()
{
    const int line_buffer_size = 1024;

    int position = 0;
    int buffer_size = line_buffer_size;
    char *buffer = malloc(sizeof(char) * buffer_size);
    
    if (!buffer)
    {
        fprintf(stderr, "hsh: Failed to allocate buffer\n");
        exit(EXIT_FAILURE);
    }

    int c;
    while (1)
    {
        c = getchar();

        if (c == EOF || c == '\n')
        {
            buffer[position] = '\0';
            return buffer;
        }
        else
        {
            buffer[position] = c;
        }
        position++;

        if (position >= buffer_size)
        {
            buffer_size += line_buffer_size;
            buffer = realloc(buffer, buffer_size);
            if (!buffer)
            {
                fprintf(stderr, "hsh: Failed to reallocate buffer\n");
                exit(EXIT_FAILURE);
            }
        }
    }
}

char **hsh_parse_line_for_args(char *line, unsigned *num)
{
    char *delim = " \t\r\n\a";
    /* 
     *  \t = tab
     *  \r = carriage return
     *  \n = newline
     *  \a = alert (bell)
     */

    int loop_end = 1;
    char **args = NULL;
    unsigned args_num = 0;
    char *token = strtok(line, delim);

    while (loop_end)
    {
        char **temp_args = realloc(args, (args_num + 1) * sizeof(char*));
        if (temp_args)
        {
            if (token)
                temp_args[args_num] = token;
            else 
            {
                temp_args[args_num] = NULL;
                loop_end = 0;
            }

            args_num++;
            args = temp_args;
        }
        else
        {
            free(args);
            fprintf(stderr, "Memory allocation failed\n");
            return NULL;
        }

        token = strtok(NULL, delim);
    }


    *num = args_num;
    return args;
}

int hsh_execute(char **args)
{
    int status;
    pid_t pid = fork();

    if (pid < 0)
    {
        perror("Fork failed");
        return 1;
    }
    else if (pid == 0)
    {
        if (execvp(args[0], args) == -1)
        {
            perror(args[0]);
            exit(1);
        }
    }
    else
    {
        waitpid(pid, &status, 0);

        if (WIFEXITED(status))
        {
            return WEXITSTATUS(status);
        }
        else
        {
            return 1;
        }
    }
    return status;
}

void hsh_interactive()
{
    char *user = getlogin();
    char host[16]; gethostname(host, sizeof(host));

    char *line;
    char **args;
    int status = 0;
    unsigned args_num = 0;

    while (1)
    {
        char *cwd = getcwd(NULL, 0);

        if (status != 0)
        {
            printf("%s@%s:%s [%d]> ", user, host, cwd, status);
        }
        else
        {
            printf("%s@%s:%s > ", user, host, cwd);
        }

        line = hsh_read_line();
        args = hsh_parse_line_for_args(line, &args_num);

        if (args)
        {
            status = hsh_execute(args);
        }
        free(line);
        free(args);
    }
}

int main()
{
    if (isatty(STDIN_FILENO))
    {
        hsh_interactive();
    }
    return 0;
}
