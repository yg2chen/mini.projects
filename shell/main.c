#include <setjmp.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#define SHLITE_RL_BUFSIZE 1024
#define SHLITE_TOK_BUFSIZE 64
#define SHLITE_TOK_DELIM " \t\r\n\a"

static jmp_buf buf;

int shlite_cd(char **args);
int shlite_help(char **args);
int shlite_exit(char **args);

char *builtin_str[] = {"cd", "help", "exit"};

int (*builtin_func[])(char **) = {&shlite_cd, &shlite_help, &shlite_exit};

int shlite_num_builtins() { return sizeof(builtin_str) / sizeof(char *); }

int shlite_cd(char **args) {
    if (args[1] == NULL) {
        fprintf(stderr, "shlite: expected arguem to \"cd\"\n");
    } else {
        if (chdir(args[1]) != 0) {
            perror("shlite");
        }
    }
    return 1;
}

int shlite_help(char **args) {
    int i;
    printf("yg2chen's crappy SHLite\n");
    printf("The following are built in:\n");

    for (i = 0; i < shlite_num_builtins(); i++) {
        printf("\t%s\n", builtin_str[i]);
    }

    printf("Use the man command for information on other programs.\n");
    return 1;
}

int shlite_exit(char **args) { return 0; }

int shlite_launch(char **args) {
    pid_t pid;
    int status;

    pid = fork();
    if (pid == 0) {
        if (execvp(args[0], args) == -1) {
            perror("shlite");
        }
        exit(EXIT_FAILURE);
    } else if (pid < 0) {
        perror("shlite");
    } else {
        do {
            waitpid(pid, &status, WUNTRACED);
        } while (!WIFEXITED(status) && !WIFSIGNALED(status));
    }

    return 1;
}

int shlite_execute(char **args) {
    int i;

    if (args[0] == NULL) {
        return 1;
    }

    for (i = 0; i < shlite_num_builtins(); i++) {
        if (strcmp(args[0], builtin_str[i]) == 0) {
            return (*builtin_func[i])(args);
        }
    }

    return shlite_launch(args);
}
char *shlite_readline() {
    int bufsize = SHLITE_RL_BUFSIZE;
    int c;
    int position = 0;
    char *buffer = malloc(bufsize * sizeof(char));
    if (!buffer) {
        fprintf(stderr, "shlite: Allocation Error\n");
        exit(EXIT_FAILURE);
    }
    while (1) {
        int c = getchar();
        if (c == EOF) {
            longjmp(buf, 1);
        } else if (c == '\n') {
            buffer[position] = '\0';
            return buffer;
        } else {
            buffer[position] = c;
        }
        position++;
        // exceeding the buffer
        if (position >= bufsize) {
            bufsize += SHLITE_RL_BUFSIZE;
            buffer = realloc(buffer, bufsize * sizeof(char));
            if (!buffer) {
                fprintf(stderr, "shlite: Reallocation Error\n");
                exit(EXIT_FAILURE);
            }
        }
    }
}

char **shlite_split_line(char *line) {
    int bufsize = SHLITE_TOK_BUFSIZE, position = 0;
    char *token;
    char **tokens = malloc(bufsize * sizeof(char *));
    char **tokens_backup;
    if (!tokens) {
        fprintf(stderr, "shlite: allocation error\n");
        exit(EXIT_FAILURE);
    }

    token = strtok(line, SHLITE_TOK_DELIM);
    while (token != NULL) {
        tokens[position++] = token;

        if (position >= bufsize) {
            bufsize += SHLITE_TOK_BUFSIZE;
            tokens_backup = tokens;
            tokens = realloc(tokens, bufsize * sizeof(char *));
            if (!tokens) {
                free(tokens_backup);
                fprintf(stderr, "shlite: Reallocation error\n");
                exit(EXIT_FAILURE);
            }
        }

        token = strtok(NULL, SHLITE_TOK_DELIM);
    }
    tokens[position] = NULL;
    return tokens;
}

void shlite_loop(void) {
    char *line;
    char **args;
    int status;

    if (setjmp(buf) == 0) {
        do {
            printf("shlite> ");
            line = shlite_readline();
            args = shlite_split_line(line);
            status = shlite_execute(args);

            free(line);
            free(args);

        } while (status);
    } else {
        return;
    }
}

int main(int argc, char **argv) {

    shlite_loop();

    return EXIT_SUCCESS;
}
