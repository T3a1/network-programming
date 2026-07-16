/*
** EPITECH PROJECT, 2026
** myftp
** File description:
** port
*/

/**
 * @file port.c
 * @brief File containing functions to execute teh PORT command
 */

#include "myftp.h"

/**
 * @brief Checks if the arguments are well-formatted
 *
 * The PORT command arguments have a very precise format and values are precise
 * Checks if all is good for execution
 *
 * @param string String containing arguments
 *
 * @return Boolean value about arguments format
 */
static bool check_subcommands(char *string)
{
    size_t length = strlen(string);
    char *token = NULL;
    unsigned int nb_components = 0;

    for (size_t i = 0; i < length; i++) {
        if (!isdigit(string[i]) && string[i] != ',')
            return false;
    }
    token = strtok(string, ",");
    while (token) {
        if (strlen(token) > 3 || strlen(token) < 1)
            return false;
        nb_components++;
        token = strtok(NULL, ",");
    }
    return (nb_components != 6) ? false : true;
}

/**
 * @brief Checks if the PORT command can be executed
 *
 * Verifies if the client has permissions to execute the PORT command
 *
 * @param curr_client Client executing the PORT command
 * @param subcommands Arguments given to the PORT command
 *
 * @return Boolean value about execution permission
 */
static bool check_context(client_t *curr_client, char *subcommands)
{
    if (!curr_client->verified) {
        write(curr_client->poll_s.fd, M530, strlen(M530));
        return false;
    }
    if (!subcommands) {
        dprintf(curr_client->poll_s.fd, M501);
        return false;
    }
    if (curr_client->transfer_mode == PASSIVE) {
        if (curr_client->data_socket.client_fd != -1)
            close(curr_client->data_socket.client_fd);
        if (curr_client->data_socket.server_fd != -1)
            close(curr_client->data_socket.server_fd);
        curr_client->data_socket.client_fd = -1;
        curr_client->data_socket.server_fd = -1;
    }
    if (curr_client->transfer_mode == ACTIVE) {
        free(curr_client->data_socket.client_infos);
    }
    return true;
}

/**
 * @brief Allocate data socket structure
 *
 * @param curr_client Client executing the PORT command
 *
 * @return Boolean value about memory allocation status
 */
static bool allocate_client_infos(client_t *curr_client)
{
    curr_client->data_socket.client_infos =
        calloc(1, sizeof(struct sockaddr_in));
    return curr_client->data_socket.client_infos != NULL;
}

/**
 * @brief Store client informations
 *
 * Parse, compute and store informations about the client's socket
 * the server must connect to
 *
 * @param curr_client Client executing the PORT command
 * @param subcommands Arguments given to the PORT command
 */
static void fill_client_infos(client_t *curr_client, char *subcommands)
{
    int h1 = 0;
    int h2 = 0;
    int h3 = 0;
    int h4 = 0;
    int p1 = 0;
    int p2 = 0;
    uint16_t port = 0;
    char client_ip[16] = {0};

    sscanf(subcommands, "%d,%d,%d,%d,%d,%d", &h1, &h2, &h3, &h4, &p1, &p2);
    port = p1 * 256 + p2;
    sprintf(client_ip, "%d.%d.%d.%d", h1, h2, h3, h4);
    curr_client->data_socket.client_infos->sin_family = AF_INET;
    curr_client->data_socket.client_infos->sin_port = htons(port);
    inet_pton(AF_INET, client_ip,
        &curr_client->data_socket.client_infos->sin_addr);
}

/**
 * @brief Handles intialization of the clients informations parsing
 *
 * @param curr_client Client executing the PORT command
 * @param subcommands Arguments given to the PORT command
 *
 * @return Boolean value about command execution
 */
static bool parse_client_infos(client_t *curr_client, char *subcommands)
{
    if (!allocate_client_infos(curr_client))
        return false;
    fill_client_infos(curr_client, subcommands);
    return true;
}

/**
 * @brief PORT command function
 *
 * @param ftp_s Pointer to the main structure
 * @param curr_client Clients executing the PORT command
 * @param subcommands Potential arguments given to the PORT command
 *
 * @return Boolean value about command execution
 */
bool port(__attribute_maybe_unused__ myftp_t *ftp_s,
    __attribute_maybe_unused__ client_t *curr_client,
    __attribute_maybe_unused__ char *subcommands)
{
    bool status = true;
    bool args_check = true;
    char *tmp_commands = NULL;

    if (!check_context(curr_client, subcommands))
        return false;
    tmp_commands = strdup(subcommands);
    args_check = check_subcommands(tmp_commands);
    free(tmp_commands);
    if (!args_check) {
        dprintf(curr_client->poll_s.fd, M501);
        return false;
    }
    parse_client_infos(curr_client, subcommands);
    curr_client->transfer_mode = ACTIVE;
    dprintf(curr_client->poll_s.fd, M200);
    return status;
}
