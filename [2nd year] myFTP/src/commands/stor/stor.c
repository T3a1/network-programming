/*
** EPITECH PROJECT, 2026
** myftp
** File description:
** stor
*/

/**
 * @file stor.c
 * @brief Functions to execute the STOR command
 */

#include "myftp.h"

/**
 * @brief STOR command function
 *
 * @param ftp_s Pointer to the main structure
 * @param curr_client Clients executing the STOR command
 * @param subcommands Potential arguments given to the STOR command
 *
 * @return Boolean value about command execution.
 */
bool stor(__attribute_maybe_unused__ myftp_t *ftp_s,
    __attribute_maybe_unused__ client_t *curr_client,
    __attribute_maybe_unused__ char *subcommands)
{
    if (!can_exec_data_tranfer(curr_client))
        return false;
    switch (curr_client->transfer_mode) {
        case PASSIVE:
            return stor_passive(curr_client, subcommands);
        case ACTIVE:
            return stor_active(curr_client, subcommands);
            return true;
        default:
            break;
    }
    return false;
}

/**
 * @brief Read data from the data socket and store it in a file
 *
 * @param curr_client Client executing the STOR command
 * @param filename File to create in order to store read data
 */
void exec_stor(client_t *curr_client, char *filename)
{
    char buffer[MAX_READ] = {0};
    int read_size = read(curr_client->data_socket.client_fd, buffer, MAX_READ);
    int fd = -1;

    if (read_size == -1) {
        perror("Reading problem");
        return;
    }
    buffer[read_size] = '\0';
    fd = open(filename, O_RDWR | O_CREAT, 0666);
    write(fd, buffer, read_size);
    close(fd);
}
