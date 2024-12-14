#include "buffered_open.h"
#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>


buffered_file_t *buffered_open(const char *pathname, int flags, ...) {
    buffered_file_t *bf = malloc(sizeof(buffered_file_t));
    bf->read_buffer = malloc(BUFFER_SIZE);
    bf->write_buffer = malloc(BUFFER_SIZE);

    if (bf == NULL || bf->read_buffer == NULL || bf->write_buffer == NULL) {
        free(bf->read_buffer);
        free(bf->write_buffer);
        free(bf);
        perror("malloc has failed");
        return NULL;
    }

    bf->flags = flags;
    bf->preappend = (flags & O_PREAPPEND) ? 1 : 0;
    bf->read_buffer_size = 0;
    bf->write_buffer_size = 0;
    bf->read_buffer_pos = 0;
    bf->write_buffer_pos = 0;    
    va_list args;
    mode_t mode = 0;

    if (flags & O_CREAT) {
        va_start(args, flags);
        mode = va_arg(args, mode_t);
        va_end(args);
    }

    bf->fd = open(pathname, flags & ~O_PREAPPEND, mode);
    if (bf->fd == -1) {
        free(bf);
        perror("open file has failed");
        return NULL;
    }

    return bf;
}

int buffered_flush(buffered_file_t *bf) {
    ssize_t written = 0;
    ssize_t total_written = 0;
    size_t to_write = bf->write_buffer_pos;


    while (to_write > 0) {

        if (bf->preappend) {
            // saving the original offset
            off_t original_offset = lseek(bf->fd, 0, SEEK_CUR);
            if (original_offset == (off_t)-1) {
                perror("lseek has failed");
                return -1;
            }

            // extracting the file size for reading the content of the whole file
            off_t file_size = lseek(bf->fd, 0, SEEK_END);
            if (file_size == (off_t)-1) {
                perror("lseek has failed");
                return -1;
            }

            char *temp_buffer = malloc(file_size);
            if (!temp_buffer) {
                perror("temp malloc for flush has failed");
                return -1;
            }

            // Read the whole file to a buffer
            lseek(bf->fd, 0, SEEK_SET);
            ssize_t read_bytes = read(bf->fd, temp_buffer, file_size);
            if (read_bytes == -1) {
                free(temp_buffer);
                perror("read has failed");
                return -1;
            }

            // Write buffered content of the buffer to the beginning of the file
            lseek(bf->fd, 0, SEEK_SET);
            written = write(bf->fd, bf->write_buffer, bf->write_buffer_pos);
            if ( written == -1) {
                free(temp_buffer);
                perror("write has failed");
                return -1;
            }

            // writing the original content next
            if (write(bf->fd, temp_buffer, read_bytes) == -1) {
                free(temp_buffer);
                perror("write has failed");
                return -1;
            }
            
            free(temp_buffer);
            lseek(bf->fd, original_offset, SEEK_SET);
        }else {
            written = write(bf->fd, bf->write_buffer + total_written, to_write);
            if (written == -1) {
                perror("write has failed");
                return -1;
            }
        }

        total_written += written;
        to_write -= written;
    }

    bf->write_buffer_pos = 0;
    
    
    return 0;
}


ssize_t buffered_write(buffered_file_t *bf, const void *buf, size_t count) 
{
    if (!bf || !buf) {
        perror("one of the arguments is NULL");
        return -1;
    }
    
    const char *data = (const char *)buf;
    size_t written = 0;


        while (written < count) {
        size_t space_left = BUFFER_SIZE - bf->write_buffer_pos;

        // If there is no space left in the buffer, flush it
        if (space_left == 0) {
            if (buffered_flush(bf) == -1) {
                perror("flush before write has failed");
                return -1;
            }
            space_left = BUFFER_SIZE;
        }
        
        // checks if there is enough space left in the buffer to copy the remaining data
        size_t to_copy = (count - written < space_left) ? count - written : space_left;

        memcpy(bf->write_buffer + bf->write_buffer_pos, data + written, to_copy);

        bf->write_buffer_pos += to_copy;
        written += to_copy;
    }
 
}


int buffered_close(buffered_file_t *bf) {
    if (!bf) {
        perror("close has failed");
        return -1;
    }

    if (buffered_flush(bf) == -1) {
        perror("close has failed");
        return -1;
    }

    int result = close(bf->fd);

    free(bf->read_buffer);
    free(bf->write_buffer);
    free(bf);

    return result;
}  

ssize_t buffered_read(buffered_file_t *bf, void *buf, size_t count) {
    if (!bf || !buf) {
        perror("one of the arguments is NULL");
        return -1;
    }

    if (buffered_flush(bf) == -1) {
        perror("flush before read has failed");
        return -1;
    }

    char *data = (char *)buf;
    size_t read_bytes = 0;

    while (read_bytes < count) {
        // If there is no data left in the buffer, read more from the file
        if (bf->read_buffer_pos == bf->read_buffer_size) {

            bf->read_buffer_size = read(bf->fd, bf->read_buffer, BUFFER_SIZE);
            if (bf->read_buffer_size == -1) {
                perror("read has failed");
                return -1;
            }
            if (bf->read_buffer_size == 0) {
                break;
            }
            bf->read_buffer_pos = 0;
        }

        // Amount of the data from the buffer to the output buffer
        size_t to_copy = (count - read_bytes < bf->read_buffer_size - bf->read_buffer_pos) ? count - read_bytes : bf->read_buffer_size - bf->read_buffer_pos;

        memcpy(data + read_bytes, bf->read_buffer + bf->read_buffer_pos, to_copy);

        bf->read_buffer_pos += to_copy;
        read_bytes += to_copy;
    }

    return read_bytes;
}