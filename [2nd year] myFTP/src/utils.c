/*
** EPITECH PROJECT, 2026
** myftp
** File description:
** utils
*/

/**
 * @file utils.c
 * @brief Utilitaries functions useful everywhere in the server code
 */

#include "myftp.h"

/**
 * @brief Initialize new client bind structure
 *
 * Fill new client bind structure with all default values
 *
 * @param new_client New clients's node
 * @param new_client_fd New client's file descriptor
 * @param tmp_pollfd Bind structure of the new client to fill
 * @param transfer Strucuture with informations on data socket
 */
void init_new_client(client_t *new_client, int new_client_fd,
    struct pollfd *tmp_pollfd, transfer_sock_t *transfer)
{
    tmp_pollfd->fd = new_client_fd;
    tmp_pollfd->events = POLLIN;
    tmp_pollfd->revents = 0;
    transfer->server_fd = -1;
    transfer->client_fd = -1;
    new_client->poll_s = *tmp_pollfd;
    new_client->data_socket = *transfer;
    new_client->next = NULL;
    new_client->username = NULL;
    new_client->verified = false;
    new_client->transfer_mode = UNDEFINED;
    new_client->data_socket.client_infos = NULL;
}

/**
 * @brief Checks if commands relying on data transfer can be executed
 *
 * @param curr_client Client executing the command
 *
 * @return Boolean value about execution possibility
 */
bool can_exec_data_tranfer(client_t *curr_client)
{
    if (!curr_client->verified) {
        write(curr_client->poll_s.fd, M530, strlen(M530));
        return false;
    }
    if (curr_client->transfer_mode == UNDEFINED) {
        write(curr_client->poll_s.fd, M425, strlen(M425));
        return false;
    }
    return true;
}

/**
 * @brief Converts the connected clients links list into an array
 *
 * Useful for poll() syscall execution
 *
 * @param ftp_s Pointer to the main structure
 */
void get_poll_array(myftp_t *ftp_s)
{
    struct pollfd *array = ftp_s->poll_arr;
    client_t *tmp = *ftp_s->clients;
    size_t index = 1;

    array[0].fd = ftp_s->control_fd;
    array[0].events = POLLIN;
    for (; tmp && index < ftp_s->nb_clients + 1; tmp = tmp->next) {
        array[index].fd = tmp->poll_s.fd;
        array[index].events = POLLIN;
        index++;
    }
}

/**
 * @brief Set all newly written revents from the array in the linked list
 *
 * @param ftp_s Pointer to the main structure
 */
void set_revents(myftp_t *ftp_s)
{
    struct pollfd *array = ftp_s->poll_arr;
    client_t *tmp = *ftp_s->clients;
    size_t index = 1;

    ftp_s->control_revents = array[0].revents;
    for (; tmp; tmp = tmp->next) {
        tmp->poll_s.revents = array[index].revents;
        array[index].fd = -1;
        index++;
    }
}
