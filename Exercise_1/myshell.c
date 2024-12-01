#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <errno.h>

#define MAX_COMMANDS 100
#define MAX_COMMAND_LEN 100

char history[MAX_COMMANDS][MAX_COMMAND_LEN];
int history_count = 0;

// add command to history
void add_to_history(const char *command) {
    if (history_count < MAX_COMMANDS) {

        strncpy(history[history_count], command, MAX_COMMAND_LEN - 1);
        history[history_count][MAX_COMMAND_LEN - 1] = '\0';
        history_count++;
    } else {

        for (int i = 1; i < MAX_COMMANDS; i++) {
            strncpy(history[i - 1], history[i], MAX_COMMAND_LEN);
        }
        strncpy(history[MAX_COMMANDS - 1], command, MAX_COMMAND_LEN - 1);
        history[MAX_COMMANDS - 1][MAX_COMMAND_LEN - 1] = '\0';
    }
}

//display command history
void print_history() {
    for (int i = 0; i < history_count; i++) {
        printf("%d: %s\n", i + 1, history[i]);
    }
}

//execute diferrent commands
void execute_command(char *command, char *paths[]) {
    pid_t pid = fork();

    if (pid < 0) {
        perror("fork failed");
        exit(EXIT_FAILURE);
    } else if (pid == 0) {
        // if inside, its the child process
        char *args[MAX_COMMAND_LEN];
        char *token = strtok(command, " ");
        int arg_count = 0;

        while (token != NULL && arg_count < MAX_COMMAND_LEN) {
            args[arg_count++] = token;
            token = strtok(NULL, " ");
        }
        args[arg_count] = NULL;

        // Check paths for command
        char exec_path[256];
        for (int i = 0; paths[i] != NULL; i++) {
            snprintf(exec_path, sizeof(exec_path), "%s/%s", paths[i], args[0]);
            execv(exec_path, args);
        }

        // execute with $PATH
        execvp(args[0], args);

        perror("exec failed");
        exit(EXIT_FAILURE);
    } else {
        // Parent process (with child pid)
        int status;
        if (waitpid(pid, &status, 0) == -1) {
            perror("wait failed");
        }
    }
}

int main(int argc, char *argv[]) {
    char *paths[MAX_COMMANDS] = {NULL};
    for (int i = 1; i < argc; i++) {
        paths[i - 1] = argv[i];
    }

    char command[MAX_COMMAND_LEN];

    while (1) {
        printf("$ ");
        fflush(stdout);

        if (fgets(command, sizeof(command), stdin) == NULL) {
            perror("fgets failed");
            continue;
        }
        command[strcspn(command, "\n")] = '\0';

        add_to_history(command);

        char *cmd = strtok(command, " ");
        if (cmd == NULL) {
            continue;
        }

        if (strcmp(cmd, "exit") == 0) {
            break;
        } else if (strcmp(cmd, "history") == 0) {
            print_history();
        } else if (strcmp(cmd, "cd") == 0) {
            char *dir = strtok(NULL, " ");
            if (dir == NULL) {
                fprintf(stderr, "cd: missing argument\n");
            } else if (chdir(dir) == -1) {
                perror("cd failed");
            }
        } else if (strcmp(cmd, "pwd") == 0) {
            char cwd[256];
            if (getcwd(cwd, sizeof(cwd)) == NULL) {
                perror("pwd failed");
            } else {
                printf("%s\n", cwd);
            }
        } else {
            execute_command(command, paths);
        }
    }

    return 0;
}