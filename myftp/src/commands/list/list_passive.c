/*
** EPITECH PROJECT, 2026
** myftp
** File description:
** list
*/

/**
 * @file list_passive.c
 * @brief Functions to execute LIST command in passive mode for data transfers.
 */

#include "myftp.h"

/**
 * @brief Execute LIST command in a fork
 *
 * Allows other connected clients to executes commands at the same time
 *
 * @param curr_client Client executing RETR command
 * @param path Path of the file where to write data to
 *
 * @return Boolean value about command execution status
 */
static bool fork_list(client_t *curr_client)
{
    pid_t cpid = fork();
    int status = 0;

    switch (cpid) {
        case -1:
            perror("fork");
            return false;
        case 0:
            close(curr_client->data_socket.server_fd);
            exec_list(curr_client);
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
 * @brief Accept incoming connections and execute LIST command
 *
 * @param curr_client Client executing LIST command
 * @return Boolean value about command execution
 */
bool list_passive(client_t *curr_client)
{
    struct sockaddr_in client_addr;
    socklen_t client_addr_size = sizeof(client_addr);

    write(curr_client->poll_s.fd, M150, strlen(M150));
    curr_client->data_socket.client_fd =
        accept(curr_client->data_socket.server_fd,
        (struct sockaddr *) &client_addr, &client_addr_size);
    if (curr_client->data_socket.client_fd == -1) {
        perror("Error accepting client connection");
        return false;
    }
    return fork_list(curr_client);
}
