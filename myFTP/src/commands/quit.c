/*
** EPITECH PROJECT, 2026
** myftp
** File description:
** quit
*/

/**
 * @file quit.c
 * @brief File containing functions to execute QUIT command
 *
 * Contains functions to properly handle QUIT command.
 * Handles removing clients from storage, and re-initializing values to
 * defaults.
 */

#include "myftp.h"

/**
 * @brief Relinks clients linked list head if nescessary
 *
 * @param ftp_s Pointer to the main structure
 * @param curr_client Client executing the QUIT command
 */
static void relink_head(myftp_t *ftp_s, client_t *curr_client)
{
    client_t *next = (*ftp_s->clients)->next;

    close(curr_client->poll_s.fd);
    if (curr_client->username)
        free(curr_client->username);
    if (curr_client->data_socket.client_fd != -1)
        close(curr_client->data_socket.client_fd);
    if (curr_client->data_socket.server_fd != -1)
        close(curr_client->data_socket.server_fd);
    free(curr_client);
    *ftp_s->clients = next;
}

/**
 * @brief Remove client node from linked list
 *
 * Properly free allocated memory and remove node from linked list
 *
 * @param ftp_s Pointer to the main strucuture
 * @param curr_client Client executing the QUIT command
 */
static void remove_client(myftp_t *ftp_s, client_t *curr_client)
{
    client_t *prev = *ftp_s->clients;

    if (curr_client == prev) {
        relink_head(ftp_s, curr_client);
        return;
    }
    for (; prev && prev->next != curr_client; prev = prev->next);
    if (!prev)
        return;
    close(curr_client->poll_s.fd);
    if (curr_client->username)
        free(curr_client->username);
    if (curr_client->data_socket.client_fd != -1)
        close(curr_client->data_socket.client_fd);
    if (curr_client->data_socket.server_fd != -1)
        close(curr_client->data_socket.server_fd);
    if (curr_client->data_socket.client_infos)
        free(curr_client->data_socket.client_infos);
    prev->next = curr_client->next;
    free(curr_client);
}

/**
 * @brief QUIT command function
 *
 * @param ftp_s Pointer to the main structure
 * @param curr_client Clients executing the QUIT command
 * @param subcommands Potential arguments given to the QUIT command
 *
 * @return Boolean value about command execution
 */
bool quit(__attribute_maybe_unused__ myftp_t *ftp_s,
    __attribute_maybe_unused__ client_t *curr_client,
    __attribute_maybe_unused__ char *subcommands)
{
    dprintf(curr_client->poll_s.fd, "%s%s", M221_A, M221_B);
    ftp_s->nb_clients--;
    remove_client(ftp_s, curr_client);
    return true;
}
