#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

void print_message(char *message, int count) {
    FILE *file = fopen("output.txt", "a");

    while (file == NULL) {
        sleep(0.5);
        file = fopen("output.txt", "a");
    }
    for (int i = 0; i < count; i++) {
        fprintf(file, "%s\n", message);
    }
    fclose(file);
}




int main(int argc, char *argv[]) {
    
    if (argc != 5) {
        fprintf(stderr, "Usage: %s <parent_message> <child1_message> <child2_message> <count>", argv[0]);
        return 1;
    }

    char *parent_message = argv[1];
    char *child1_message = argv[2];
    char *child2_message = argv[3];
    int count = atoi(argv[4]);

    pid_t pid2;
    pid_t pid1 = fork();

    if (pid1 < 0) {
        perror("fork failed");
        exit(1);
    } else if (pid1 == 0) {
        // Child 1
        print_message(child1_message, count);
        exit(0);
    } else {
        // Parent process
        pid2 = fork();
        if (pid2 < 0) {
            perror("fork failed");
            exit(1);
        } else if (pid2 == 0) {
            // Child 2
            sleep(1);
            print_message(child2_message, count);
            exit(0);
        }
    }

    if (wait(NULL) != -1 && wait(NULL) != -1) {
        print_message(parent_message, count);
    } else {
        perror("wait failed");
        
    }
}