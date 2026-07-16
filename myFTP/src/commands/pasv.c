/*
** EPITECH PROJECT, 2026
** myftp
** File description:
** pasv
*/

/**
 * @file pasv.c
 * @brief Function to execute the PASV command
 */

#include "myftp.h"

/**
 * @brief Inform client that data socket has been successfully opended
 *
 * @param curr_client Client executing the PASV command
 */
static void respond(client_t *curr_client)
{
    struct sockaddr_in server_addr;
    socklen_t server_addr_len = sizeof(server_addr);
    uint8_t *ip_bytes = NULL;
    uint16_t port = 0;
    uint8_t port1 = 0;
    uint8_t port2 = 0;
    char string[MAX_READ] = {0};

    getsockname(curr_client->data_socket.server_fd,
        (struct sockaddr *)&server_addr, &server_addr_len);
    ip_bytes = (uint8_t *)&server_addr.sin_addr.s_addr;
    port = ntohs(server_addr.sin_port);
    port1 = port / 256;
    port2 = port % 256;
    sprintf(string, "227 Entering Passive Mode "
        "(%d,%d,%d,%d,%d,%d)\r\n",
        ip_bytes[0], ip_bytes[1], ip_bytes[2], ip_bytes[3], port1, port2);
    write(curr_client->poll_s.fd,
        string, strlen(string));
}

/**
 * @brief Data socket creation and configuration by the server
 *
 * @param curr_client Client executing the PASV command
 * @param bind_s Structure containing server data socket informations
 * @param addr_size_s Size of the bind_s structure
 *
 * @return Boolean value a socket creation status
 */
static bool create_data_socket(client_t *curr_client,
    struct sockaddr_in *bind_s, socklen_t *addr_size_s)
{
    curr_client->data_socket.server_fd = socket(AF_INET, SOCK_STREAM, 0);
    curr_client->data_socket.client_infos = NULL;
    if (curr_client->data_socket.server_fd == -1) {
        perror("Socket creation");
        return false;
    }
    if (!bind_socket(
            curr_client->data_socket.server_fd, bind_s, addr_size_s))
        return false;
    if (!listen_socket(curr_client->data_socket.server_fd))
        return false;
    curr_client->transfer_mode = PASSIVE;
    return true;
}

/**
 * @brief Checks if the client has permissions to execute the PASV command
 *
 * @param curr_client Client executing the PASV command
 */
static bool check_context(client_t *curr_client)
{
    if (!curr_client->verified) {
        write(curr_client->poll_s.fd, M530, strlen(M530));
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
 * @brief PASV command function
 *
 * @param ftp_s Pointer to the main structure
 * @param curr_client Clients executing the PASV command
 * @param subcommands Potential arguments given to the PASV command
 *
 * @return Boolean value about command execution
 */
bool pasv(__attribute_maybe_unused__ myftp_t *ftp_s,
    __attribute_maybe_unused__ client_t *curr_client,
    __attribute_maybe_unused__ char *subcommands)
{
    struct sockaddr_in bind_s;
    socklen_t addr_size_s;
    bool status = true;

    if (!check_context(curr_client))
        return false;
    bind_s.sin_family = AF_INET;
    bind_s.sin_port = htons(0);
    bind_s.sin_addr.s_addr = INADDR_ANY;
    addr_size_s = sizeof(struct sockaddr_in);
    status = create_data_socket(curr_client, &bind_s, &addr_size_s);
    respond(curr_client);
    return status;
}
