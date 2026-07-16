/*
** EPITECH PROJECT, 2026
** myftp
** File description:
** list_active
*/

/**
 * @file list_active.c
 * @brief Function to execute LIST command in active mode for data transfers.
 */

#include "myftp.h"

/**
 * @brief Execute LIST command in a fork
 *
 * Allows other connected clients to executes commands at the same time
 *
 * @param curr_client Client executing LIST command
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
            exec_list(curr_client);
            close(curr_client->data_socket.client_fd);
            exit(EXIT_FAILURE);
        default:
            close(curr_client->data_socket.client_fd);
            waitpid(cpid, &status, 0);
            dprintf(curr_client->poll_s.fd, "%s%s%s", M226_A, M226_B, M226_C);
            break;
    }
    return true;
}

/**
 * @brief Connect to client's data socket and execute LIST command
 *
 * @param curr_client Client executing the LIST command
 *
 * @return Boolean value about command execution
 */
bool list_active(client_t *curr_client)
{
    curr_client->data_socket.client_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (connect(curr_client->data_socket.client_fd,
            (struct sockaddr *)curr_client->data_socket.client_infos,
            sizeof(*curr_client->data_socket.client_infos)) == -1) {
        perror("Connect");
        dprintf(curr_client->poll_s.fd, "%s", M425);
        close(curr_client->data_socket.client_fd);
        curr_client->data_socket.client_fd = -1;
        return false;
    }
    write(curr_client->poll_s.fd, M150, strlen(M150));
    fork_list(curr_client);
    return true;
}
