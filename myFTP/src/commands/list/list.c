/*
** EPITECH PROJECT, 2026
** myftp
** File description:
** list
*/

#include "myftp.h"

/**
 * @brief RETR command function
 *
 * @param ftp_s Pointer to the main structure
 * @param curr_client Clients executing the RETR command
 * @param subcommands Potential arguments given to the RETR command
 *
 * @return Boolean value about command execution.
 */
bool list(__attribute_maybe_unused__ myftp_t *ftp_s,
    __attribute_maybe_unused__ client_t *curr_client,
    __attribute_maybe_unused__ char *subcommands)
{
    if (!can_exec_data_tranfer(curr_client))
        return false;
    switch (curr_client->transfer_mode) {
        case PASSIVE:
            return list_passive(curr_client);
        case ACTIVE:
            return list_active(curr_client);
        default:
            break;
    }
    return false;
}

/**
 * @brief Execute execlp syscall to execute ls -l command
 *
 * @param curr_client Client executing LIST commmand
 */
void exec_list(client_t *curr_client)
{
    dup2(curr_client->data_socket.client_fd, STDOUT_FILENO);
    dup2(curr_client->data_socket.client_fd, STDERR_FILENO);
    execlp("ls", "ls", "-l", NULL);
}
