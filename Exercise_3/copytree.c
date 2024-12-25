#ifndef COPYTREE_H
#define COPYTREE_H

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>



void copy_file(const char *src, const char *dest, int copy_symlinks, int copy_permissions) {
    int src_fd, dest_fd; 
    char buffer[4096];   
    ssize_t bytes_read, bytes_written;

    src_fd = open(src, O_RDONLY);
    if (src_fd == -1) {
        perror("COMMAND failed");
        exit(EXIT_FAILURE);
    }

    if (copy_symlinks) {
        char link_target[4096];
        ssize_t link_length = readlink(src, link_target, sizeof(link_target));

        //if it fails, it is not a symlink. so we just copy the file as it is
        if (link_length > 0) {
            link_target[link_length] = '\0';
            if (symlink(link_target, dest) == -1) {
                perror("COMMAND failed");
                close(src_fd);
                exit(EXIT_FAILURE);
            }
            close(src_fd);
            return;
        }
    }

    dest_fd = open(dest, O_WRONLY | O_CREAT | O_TRUNC, 0644); 
    if (dest_fd == -1) {
        perror("COMMAND failed");
        close(src_fd);
        exit(EXIT_FAILURE);
    }

    while ((bytes_read = read(src_fd, buffer, sizeof(buffer))) > 0) {
        bytes_written = write(dest_fd, buffer, bytes_read);
        if (bytes_written != bytes_read) {
            perror("COMMAND failed");
            close(src_fd);
            close(dest_fd);
            exit(EXIT_FAILURE);
        }
    }

    struct stat src_stat;
    if (fstat(src_fd, &src_stat) == -1) {
        perror("COMMAND failed");
        close(src_fd);
        exit(EXIT_FAILURE);
    }

    if (copy_permissions) {
        if (fchmod(dest_fd, src_stat.st_mode) == -1) {
            perror("COMMAND failed");
            close(src_fd);
            close(dest_fd);
            exit(EXIT_FAILURE);
        }
    }
}

void copy_directory(const char *src, const char *dest, int copy_symlinks, int copy_permissions) {

}
#endif // COPYTREE_H