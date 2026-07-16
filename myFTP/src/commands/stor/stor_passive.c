/*
** EPITECH PROJECT, 2026
** myftp
** File description:
** stor_passive
*/

/**
 * @file stor_passive.c
 * @brief Functions to execute STOR using passive mode for data transfers.
 */

#include "myftp.h"

/**
 * @brief Execute STOR command in a fork
 *
 * Allows other connected clients to executes commands at the same time
 *
 * @param curr_client Client executing STOR command
 * @param path Path of the file where to write data to
 * @return Boolean value about command execution status
 */
static bool fork_stor(client_t *curr_client, char *path)
{
    pid_t cpid = fork();
    int status = 0;

    switch (cpid) {
        case -1:
            perror("fork");
            return false;
        case 0:
            close(curr_client->data_socket.server_fd);
            exec_stor(curr_client, path);
            exit(EXIT_FAILURE);
        default:
            close(curr_client->data_socket.client_fd);
            waitpid(cpid, &status, 0);
            dprintf(curr_client->poll_s.fd, "%s%s%s",
                M226_A, M226_B, M226_C);
            break;
    }
    return true;
}

/**
 * @brief Accepts incoming connection to data socket and executes STOR command
 *
 * @param curr_client Client executing the STOR command
 * @param subcommands Argument(s) given to the STOR command
 *
 * @return Boolean value about command execution status
 */
bool stor_passive(client_t *curr_client, char *subcommands)
{
    struct sockaddr_in client_addr;
    socklen_t client_addr_size = sizeof(client_addr);

    if (!subcommands) {
        dprintf(curr_client->poll_s.fd, "%s", M501);
        return false;
    }
    write(curr_client->poll_s.fd, M150, strlen(M150));
    curr_client->data_socket.client_fd =
        accept(curr_client->data_socket.server_fd,
        (struct sockaddr *) &client_addr, &client_addr_size);
    if (curr_client->data_socket.client_fd == -1) {
        perror("Error accepting client connection");
        return false;
    }
    return fork_stor(curr_client, subcommands);
}
