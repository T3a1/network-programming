/*
** EPITECH PROJECT, 2026
** myftp
** File description:
** server
*/

/**
 * @file server.c
 * @brief File containing server utils
 *
 * Contains function to create the control socket with ths functions to
 * properly configure it.
 */

#include "myftp.h"

/**
 * @brief Bind control socket
 *
 * Give a name to the control socket
 *
 * @param fd File descriptor of the control socket
 * @param bind_s Structure to bind the control socket with
 * @param addr_size_s Size of the binded structure
 */
bool bind_socket(int fd, struct sockaddr_in *bind_s,
    socklen_t *addr_size_s)
{
    if (bind(fd, (struct sockaddr *) bind_s, *addr_size_s) == -1) {
        perror("Bind");
        close(fd);
        return false;
    }
    return true;
}

/**
 * Set control socket to listen mode
 *
 * @param fd File descriptor of the socket
 *
 * @return Boolean to give status of socket listening
 */
bool listen_socket(int fd)
{
    if (listen(fd, MAX_CONNECTIONS) == -1) {
        perror("Socket listening");
        return false;
    }
    return true;
}

/**
 * @brief Open control socket
 *
 * Link socket to a file descriptor, bind it and set it to LISTEN mode
 *
 * @param ftp_s Pointer to the main structure
 * @param bind_s Structure to bind the control socket with
 * @param addr_size_s Size of the binded structure
 */
static void create_data_socket(myftp_t *ftp_s,
    struct sockaddr_in *bind_s, socklen_t *addr_size_s)
{
    ftp_s->control_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (ftp_s->control_fd == -1) {
        perror("Socket creation");
        ftp_s->exit_status = EXIT_FAILURE;
        return;
    }
    if (!bind_socket(ftp_s->control_fd, bind_s, addr_size_s)) {
        ftp_s->exit_status = EXIT_FAILURE;
        return;
    }
    if (!listen_socket(ftp_s->control_fd)) {
        ftp_s->exit_status = EXIT_FAILURE;
        return;
    }
}

/**
 * @brief Initialize control socket informations
 *
 * Fill control socket structures with informations
 * @param ftp_s Pointer to the main structure
 * @param port FTP server port
 */
void initialize_server(myftp_t *ftp_s, int port)
{
    struct sockaddr_in bind_s;
    socklen_t addr_size_s;

    bind_s.sin_family = AF_INET;
    bind_s.sin_port = htons(port);
    bind_s.sin_addr.s_addr = INADDR_ANY;
    addr_size_s = sizeof(struct sockaddr_in);
    create_data_socket(ftp_s, &bind_s, &addr_size_s);
}
