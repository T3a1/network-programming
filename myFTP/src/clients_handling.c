/*
** EPITECH PROJECT, 2026
** myftp
** File description:
** clients_handling
*/

/**
 * @file clienys_handling.c
 * @brief File with server main loop
 *
 * Contains the nescessary functions to hanble the FTP server main loop.
 * Also handles the incoming connections, signal catching, data reading.
 * Given the circumstances, will properly handle clients and send data
 * where needed in the global code.
 */

#include "myftp.h"

/**
 * @brief Variable that runs the server main loop
 *
 * This global variable can only be modified by catching the SIGINT signal
 * It will directly be handled bu the signal() and the signal_handler()
 * functions
 */
static volatile bool run_loop = true;

/**
 * @brief Handle SIGINT signal
 *
 * When SIGINT signal is received, stop the server main loop in order
 * to eject all remaining clients
 *
 * @param signum Caught signal
 */
static void signal_handler(__attribute_maybe_unused__ int signum)
{
    run_loop = false;
}

/**
 * @brief Add a client node
 *
 * Creates a new node in the linked list of connected clients
 *
 * @param ftp_s Pointer to the main structure
 * @param new_client New node to add in the linked list
 */
static void add_client(myftp_t *ftp_s, client_t *new_client)
{
    new_client->next = *ftp_s->clients;
    *ftp_s->clients = new_client;
    ftp_s->nb_clients++;
}

/**
 * @brief Store new connected client informations
 *
 * @param ftp_s Pointer to the main structure
 * @param new_client_fd File descriptor of the new client
 * @param client_addr Bind structure with new client informations
 */
static void init_new_conn(myftp_t *ftp_s, int new_client_fd,
    struct sockaddr_in *client_addr)
{
    client_t *new_client = calloc(1, sizeof(client_t));
    struct pollfd tmp_pollfd;
    transfer_sock_t transfer;

    if (ftp_s->nb_clients == MAX_CONNECTIONS) {
        fprintf(stderr, "Cannot accept more clients\n");
        return;
    }
    init_new_client(new_client, new_client_fd, &tmp_pollfd, &transfer);
    printf("Nouvelle connexion depuis %s:%d\n",
        inet_ntoa(client_addr->sin_addr), ntohs(client_addr->sin_port));
    write(new_client_fd, M220, strlen(M220));
    add_client(ftp_s, new_client);
}

/**
 * @brief Read and interpret received data
 *
 * Act as nescessary given the received data
 *
 * @param ftp_s Pointer to the main structure
 * @param tmp_client Client we're reading data from
 * @param buffer Buffer containing received data
 * @param read_bytes Number of bytes read
 */
static void handle_client_read(myftp_t *ftp_s, client_t *tmp_client,
    char *buffer, int read_bytes)
{
    bool only_spaces = true;

    if (read_bytes <= 1) {
        quit(ftp_s, tmp_client, NULL);
        return;
    }
    for (int i = 0; i < read_bytes - 2; i++) {
        if (buffer[i] != ' ' && buffer[i] != '\t')
            only_spaces = false;
    }
    if (only_spaces) {
        handle_unknown_command(ftp_s, tmp_client);
        return;
    }
    buffer[strlen(buffer) - 2] = '\0';
    execute_command(ftp_s, tmp_client, buffer);
}

/**
 * @brief Read received data
 *
 * When data is received, read it and prepare its interpretation
 *
 * @param ftp_s Pointer to the main structure
 */
static void read_data_and_disconnections(myftp_t *ftp_s)
{
    client_t *next_client = NULL;
    char buffer[MAX_READ];
    int read_bytes = 0;
    client_t *tmp_client = *ftp_s->clients;

    while (tmp_client) {
        next_client = tmp_client->next;
        if (tmp_client->poll_s.revents & POLLIN) {
            memset(buffer, 0, MAX_READ - 1);
            read_bytes = read(tmp_client->poll_s.fd, buffer, MAX_READ - 1);
            handle_client_read(ftp_s, tmp_client, buffer, read_bytes);
        }
        tmp_client = next_client;
    }
}

/**
 * @brief Handle all connection events on the server
 *
 * Format main array to get informations.
 * Analyze each clients to see if data was received
 * Accept incoming connections
 *
 * @param ftp_s Pointer to the main structure
 */
static void handle_events(myftp_t *ftp_s)
{
    int new_client_fd = -1;
    struct sockaddr_in client_addr;
    socklen_t client_addr_len = sizeof(client_addr);

    while (run_loop) {
        get_poll_array(ftp_s);
        poll(ftp_s->poll_arr, ftp_s->nb_clients + 1, -1);
        set_revents(ftp_s);
        if (ftp_s->control_revents & POLLIN) {
            client_addr_len = sizeof(client_addr);
            new_client_fd = accept(ftp_s->control_fd,
                (struct sockaddr *)&client_addr, &client_addr_len);
            init_new_conn(ftp_s, new_client_fd, &client_addr);
        }
        if (ftp_s->exit_status == EXIT_FAILURE)
            return;
        new_client_fd = -1;
        read_data_and_disconnections(ftp_s);
    }
}

/**
 * @brief Extract server informations
 *
 * Display server informations from the binded structure
 *
 * @param ftp_s Pointer to the main structure
 */
static void initialize_poll_structures(myftp_t *ftp_s)
{
    struct sockaddr_in new_addr;
    socklen_t addr_len = sizeof(new_addr);

    getsockname(ftp_s->control_fd,
        (struct sockaddr *)&new_addr, &addr_len);
    printf("Used port : %d\n", ntohs(new_addr.sin_port));
    signal(SIGINT, signal_handler);
}

/**
 * @brief Eject all currently connected clients
 *
 * Close every clients file descriptors
 *
 * @param ftp_s Pointer to the main structure
 */
static void eject_clients(myftp_t *ftp_s)
{
    client_t *tmp_client = *ftp_s->clients;
    client_t *to_delete = *ftp_s->clients;

    while (tmp_client) {
        to_delete = tmp_client;
        tmp_client = tmp_client->next;
        if (to_delete->username)
            free(to_delete->username);
        if (to_delete->data_socket.client_fd != -1)
            close(to_delete->data_socket.client_fd);
        if (to_delete->data_socket.server_fd != -1)
            close(to_delete->data_socket.server_fd);
        if (to_delete->data_socket.client_infos)
            free(to_delete->data_socket.client_infos);
        close(to_delete->poll_s.fd);
        free(to_delete);
    }
}

/**
 * @brief Main clients handling function
 *
 * FTP server order of action
 * First initialize poll structures.
 * Then accept and manage clients
 * Finally, if the server stops, properly eject all connected clients
 *
 * @param ftp_s Pointer to the main structure
 */
void handle_clients(myftp_t *ftp_s)
{
    initialize_poll_structures(ftp_s);
    handle_events(ftp_s);
    eject_clients(ftp_s);
}
