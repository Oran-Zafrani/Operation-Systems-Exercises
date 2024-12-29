#ifndef COPYTREE_H
#define COPYTREE_H

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <dirent.h>
#include <string.h>
#include <errno.h>


void copy_file(const char *src, const char *dest, int copy_symlinks, int copy_permissions) {
    int src_fd, dest_fd; 
    char buffer[4096];   
    ssize_t bytes_read, bytes_written;

    src_fd = open(src, O_RDONLY);
    if (src_fd == -1) {
        perror("open file failed");
        exit(EXIT_FAILURE);
    }

    if (copy_symlinks) {
        char link_target[4096];
        ssize_t link_length = readlink(src, link_target, sizeof(link_target));

        //if it fails, it is not a symlink. so we just copy the file as it is
        if (link_length > 0) {
            link_target[link_length] = '\0';
            if (symlink(link_target, dest) == -1) {
                perror("symlink failed");
                close(src_fd);
                exit(EXIT_FAILURE);
            }
            close(src_fd);
            return;
        }
    }

    dest_fd = open(dest, O_WRONLY | O_CREAT | O_TRUNC, 0644); 
    if (dest_fd == -1) {
        perror("open file failed");
        close(src_fd);
        exit(EXIT_FAILURE);
    }

    while ((bytes_read = read(src_fd, buffer, sizeof(buffer))) > 0) {
        bytes_written = write(dest_fd, buffer, bytes_read);
        if (bytes_written != bytes_read) {
            perror("file write failed");
            close(src_fd);
            close(dest_fd);
            exit(EXIT_FAILURE);
        }
    }

    struct stat src_stat;
    if (fstat(src_fd, &src_stat) == -1) {
        perror("file fstat failed");
        close(src_fd);
        exit(EXIT_FAILURE);
    }

    if (copy_permissions) {
        if (fchmod(dest_fd, src_stat.st_mode) == -1) {
            perror("file fchmod failed");
            close(src_fd);
            close(dest_fd);
            exit(EXIT_FAILURE);
        }
    }
}

void create_directories(const char *src, const char *dest, int copy_permissions) {
    struct stat src_stat;
    char temp_path[1024];
    char *token = NULL;
    char current_path[1024] = "";

    // Copy the path to a temporary buffer
    strncpy(temp_path, dest, sizeof(temp_path) - 1);
    temp_path[sizeof(temp_path) - 1] = '\0';

    // Use strtok to split the path by "/"
    token = strtok(temp_path, "/");
    while (token != NULL) {
        // Append the token to the current path
        strcat(current_path, token);
        strcat(current_path, "/");

        // Create the current directory if it doesn't exist
        if (mkdir(current_path, 0755) != 0 && errno != EEXIST) {
        perror("make directory failed");
        exit(EXIT_FAILURE);
        }

        // Get the next token
        token = strtok(NULL, "/");
    }


    if (stat(src, &src_stat) == -1) {
        perror("file stat failed");
        exit(EXIT_FAILURE);
    }

    if (copy_permissions) {
        if (chmod(dest, src_stat.st_mode) == -1) {
            perror("file chmod failed");
            exit(EXIT_FAILURE);
        }
    }

    DIR *dir;
    struct dirent *entry;

    if ((dir = opendir(src)) == NULL) {
        perror("open directory failed");
        exit(EXIT_FAILURE);
    }

    while ((entry = readdir(dir)) != NULL) {
        char src_path[4096];
        char dest_path[4096];

        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
            continue;
        }
        
        src_path[0] = '\0';
        strcpy(src_path, src);
        strcat(src_path, "/");
        strcat(src_path, entry->d_name);

        struct stat entry_stat;
        if (stat(src_path, &entry_stat) == -1) {
            perror("file stat failed");
            closedir(dir);
            exit(EXIT_FAILURE);
        }


        if (S_ISDIR(entry_stat.st_mode)) {

            dest_path[0] = '\0';
            strcpy(dest_path, dest);
            strcat(dest_path, "/");
            strcat(dest_path, entry->d_name);

            create_directories(src_path, dest_path, copy_permissions);
        }
    }

    closedir(dir);
}


void copy_directory(const char *src, const char *dest, int copy_symlinks, int copy_permissions) {

    create_directories(src, dest, copy_permissions);
    copy_file_recursive(src, dest, copy_symlinks, copy_permissions);
}


void copy_file_recursive(const char *src, const char *dest, int copy_symlinks, int copy_permissions) {
    DIR *dir;
    struct dirent *entry;

    if ((dir = opendir(src)) == NULL) {
        perror("open directory failed");
        exit(EXIT_FAILURE);
    }

    while ((entry = readdir(dir)) != NULL) {
        char src_path[4096];
        char dest_path[4096];

        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
            continue;
        }

        src_path[0] = '\0';
        strcpy(src_path, src);
        strcat(src_path, "/");
        strcat(src_path, entry->d_name);

        dest_path[0] = '\0';
        strcpy(dest_path, dest);
        strcat(dest_path, "/");
        strcat(dest_path, entry->d_name);

        struct stat entry_stat;
        if (stat(src_path, &entry_stat) == -1) {
            perror("file stat failed");
            closedir(dir);
            exit(EXIT_FAILURE);
        }

        if (S_ISDIR(entry_stat.st_mode)) {
            copy_file_recursive(src_path, dest_path, copy_symlinks, copy_permissions);
        } else {
            copy_file(src_path, dest_path, copy_symlinks, copy_permissions);
        }
    }

    closedir(dir);
}
#endif // COPYTREE_H