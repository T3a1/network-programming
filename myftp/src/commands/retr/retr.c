/*
** EPITECH PROJECT, 2026
** myftp
** File description:
** retr
*/

/**
 * @file retr.c
 * @brief Functions to execute the RETR command
 */

#include "myftp.h"

/**
 * @brief Checks if file is good to be treated by a command
 *
 * @param path Path to the file
 * @return Boolean about file status
 */
bool is_file(char *path)
{
    struct stat sb;

    if (!path) {
        return false;
    }
    if (lstat(path, &sb) == -1) {
        return false;
    }
    if (S_ISDIR(sb.st_mode)) {
        return false;
    }
    if (access(path, R_OK | W_OK) == -1) {
        return false;
    }
    return S_ISREG(sb.st_mode);
}

/**
 * @brief Get file size
 *
 * @param path Path to the file
 * @return File size
 */
size_t get_file_size(char *path)
{
    struct stat sb;

    lstat(path, &sb);
    return sb.st_size;
}

/**
 * @brief RETR command function
 *
 * @param ftp_s Pointer to the main structure
 * @param curr_client Clients executing the RETR command
 * @param subcommands Potential arguments given to the RETR command
 *
 * @return Boolean value about command execution.
 */
bool retr(__attribute_maybe_unused__ myftp_t *ftp_s,
    __attribute_maybe_unused__ client_t *curr_client,
    __attribute_maybe_unused__ char *subcommands)
{
    if (!can_exec_data_tranfer(curr_client))
        return false;
    switch (curr_client->transfer_mode) {
        case PASSIVE:
            return retr_passive(curr_client, subcommands);
        case ACTIVE:
            return retr_active(curr_client, subcommands);
            return true;
        default:
            break;
    }
    return false;
}

/**
 * @brief Read a file and writes its content in the data socket
 *
 * @param curr_client Client executing the RETR command
 * @param path Path of the file from which to read content
 */
void exec_retr(client_t *curr_client, char *path)
{
    size_t file_size = get_file_size(path);
    char *buffer = calloc(file_size + 1, sizeof(char));
    int fd = -1;

    if (!buffer)
        return;
    buffer[file_size] = '\0';
    fd = open(path, O_RDONLY);
    read(fd, buffer, file_size);
    write(curr_client->data_socket.client_fd, buffer, file_size);
    close(fd);
    free(buffer);
}
