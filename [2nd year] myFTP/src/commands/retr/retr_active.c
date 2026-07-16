/*
** EPITECH PROJECT, 2026
** myftp
** File description:
** retr_active
*/

/**
 * @file retr_active.c
 * @brief Functions to execute RETR command in active mode for data transfers.
 */

#include "myftp.h"

/**
 * @brief Execute RETR command in a fork
 *
 * Allows other connected clients to executes commands at the same time
 *
 * @param curr_client Client executing RETR command
 * @param path Path of the file where to write data to
 *
 * @return Boolean value about command execution status
 */
static bool fork_retr(client_t *curr_client, char *path)
{
    pid_t cpid = fork();
    int status = 0;

    switch (cpid) {
        case -1:
            perror("fork");
            return false;
        case 0:
            exec_retr(curr_client, path);
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
 * @brief Connect to client data socket and execute RETR command
 *
 * @param curr_client Client executing the RETR command
 * @param subcommands Arguments given to RETR command
 *
 * @return Boolean value about command execution
 */
bool retr_active(client_t *curr_client, char *subcommands)
{
    if (!is_file(subcommands)) {
        dprintf(curr_client->poll_s.fd, "%s", M550);
        return false;
    }
    write(curr_client->poll_s.fd, M150, strlen(M150));
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
    fork_retr(curr_client, subcommands);
    return true;
}
