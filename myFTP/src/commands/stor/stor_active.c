/*
** EPITECH PROJECT, 2026
** myftp
** File description:
** stor_active
*/

#include "myftp.h"

/**
 * @brief Execute STOR command in a fork
 *
 * Allows other connected clients to executes commands at the same time
 *
 * @param curr_client Client executing STOR command
 * @param path Path of the file where to write data to
 *
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
            exec_stor(curr_client, path);
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
 * @brief Connect to client data socket and execute STOR command
 *
 * @param curr_client Client executing the STOR command
 * @param subcommands Argument(s) given to the STOR command
 *
 * @return Boolean value about command execution status
 */
bool stor_active(client_t *curr_client, char *subcommands)
{
    if (!subcommands) {
        dprintf(curr_client->poll_s.fd, "%s", M501);
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
    fork_stor(curr_client, subcommands);
    return true;
}
