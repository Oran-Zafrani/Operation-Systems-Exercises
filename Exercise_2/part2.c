#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/wait.h>

void write_message(const char *message, int count) {
    for (int i = 0; i < count; i++) {
        printf("%s\n", message);
        usleep((rand() % 100) * 1000); // Random delay between 0 and 99 milliseconds
    }
}

void acquire_lock(const char *lockfile) {
    while (1) {
        int fd = open(lockfile, O_CREAT | O_EXCL, 0666); 
        if (fd == -1) {
            if (errno == EEXIST) {
                // Lockfile exists, wait and retry
                usleep(1000); 
                continue;
            } else {
                perror("Error acquiring lock");
                exit(1);
            }
        }
        close(fd); 
        break;
    }
}

void release_lock(const char *lockfile) {
    if (unlink(lockfile) == -1) {
        perror("Error releasing lock");
        exit(1);
    }
}

int main(int argc, char *argv[]) {

    if (argc <= 4) {
        fprintf(stderr, "Usage: %s <message1> <message2> ... <count>", argv[0]);
        return 1;
    }

    int numThreads = argc - 2;
    int count = atoi(argv[argc - 1]);



    if (count <= 0) {
        fprintf(stderr, "Error: write count must be greater than 0.\n");
        return 1;
    }

    const char *lockfile = "lockfile.lock"; 
    pid_t pids[numThreads]; 

    for (int i = 0; i < numThreads; i++) {
        pids[i] = fork();
        if (pids[i] < 0) {
            perror("fork error");
            return 1;
        }

        if (pids[i] == 0) {
            // Child process
            for (int j = 0; j < count; j++) {
                acquire_lock(lockfile); 

                write_message(argv[i + 1], 1);

                release_lock(lockfile); 
            }
            exit(0); 
        }
    }

        // Parent process wait for all child processes to complete
    for (int i = 0; i < count; i++) {
        if (waitpid(pids[i], NULL, 0) == -1) {
            fprintf(stderr, "wait pid %d failed", pids[i]);
            return 1;
        }
    }

    return 0;

}
