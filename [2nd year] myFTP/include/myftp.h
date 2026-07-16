/*
** EPITECH PROJECT, 2026
** myftp
** File description:
** myftp
*/

#ifndef MYFTP_H_
    #define MYFTP_H_
    #include <unistd.h>
    #include <stdlib.h>
    #include <stdio.h>
    #include <stdbool.h>
    #include <sys/socket.h>
    #include <netinet/in.h>
    #include <arpa/inet.h>
    #include <string.h>
    #include <sys/wait.h>
    #include <signal.h>
    #include <poll.h>
    #include <ctype.h>
    #include <sys/stat.h>
    #include <sys/sysmacros.h>
    #include <fcntl.h>

    #undef EXIT_FAILURE
    #define EXIT_FAILURE 84
    #define UNKNOWN_COMMAND 10
    #define MAX_CONNECTIONS 50000
    #define MAX_READ 4096

    #define M150 "150 File status okay; about to open data connection.\r\n"

    #define M200 "200 Command okay.\r\n"
    #define M221_A "221 Service closing control connection.\n"
    #define M221_B "    Logged out if appropriate.\r\n"
    #define M220 "220 Service ready for new user.\r\n"
    #define M226_A "226 Closing data connection.\n"
    #define M226_B "    Requested file action successful (for example, file\n"
    #define M226_C "    transfer or file abort).\r\n"
    #define M230 "230 User logged in, proceed.\r\n"
    #define M250 "250 Requested file action okay, completed.\r\n"

    #define M331 "331 User name okay, need password.\r\n"
    #define M332 "332 Need account for login.\r\n"

    #define M425 "425 Can't open data connection.\r\n"

    #define M500_A "500 Syntax error, command unrecognized and the requested\n"
    #define M500_B "    action did not take place. This may include\n"
    #define M500_C "    errors such as command line too long.\r\n"
    #define M501 "501 Syntax error in parameters or arguments.\r\n"
    #define M503 "503 Bad sequence of commands.\r\n"
    #define M530 "530 Not logged in.\r\n"
    #define M550 "550 Requested action not taken. Wrong file or perms.\r\n"

typedef enum {
    PASSIVE,
    ACTIVE,
    UNDEFINED
} transfer_t;

typedef struct {
    int server_fd;
    int client_fd;
    struct sockaddr_in *client_infos;
} transfer_sock_t;

typedef struct client_s {
    char *username;
    bool verified;
    struct pollfd poll_s;
    transfer_t transfer_mode;
    transfer_sock_t data_socket;
    struct client_s *next;
} client_t;

typedef struct {
    int exit_status;
    client_t **clients;
    size_t nb_clients;
    int control_fd;
    short int control_revents;
    struct pollfd *poll_arr;
    char *working_dir;
    char *absolute_root;
} myftp_t;

struct ftp_commands {
    char *command_name;
    bool (*function)(myftp_t *ftp_s,
        client_t *curr_client, char *subcommands);
};

struct ftp_help {
    char *command_name;
    char *command_description;
};

// Server handling
void initialize_server(myftp_t *ftp_s, int port);
void execute_command(myftp_t *ftp_s, client_t *curr_client, char *buffer);
bool bind_socket(int fd, struct sockaddr_in *bind_s,
    socklen_t *addr_size_s);
bool listen_socket(int fd);

// Clients handling
void handle_clients(myftp_t *ftp_s);


// Commands
bool user(__attribute_maybe_unused__ myftp_t *ftp_s,
    __attribute_maybe_unused__ client_t *curr_client,
    __attribute_maybe_unused__ char *subcommands);

bool quit(__attribute_maybe_unused__ myftp_t *ftp_sure,
    __attribute_maybe_unused__ client_t *curr_client,
    __attribute_maybe_unused__ char *subcommands);

bool pass(__attribute_maybe_unused__ myftp_t *ftp_s,
    __attribute_maybe_unused__ client_t *curr_client,
    __attribute_maybe_unused__ char *subcommands);

bool help(__attribute_maybe_unused__ myftp_t *ftp_s,
    __attribute_maybe_unused__ client_t *curr_client,
    __attribute_maybe_unused__ char *subcommands);

bool list(__attribute_maybe_unused__ myftp_t *ftp_s,
    __attribute_maybe_unused__ client_t *curr_client,
    __attribute_maybe_unused__ char *subcommands);
bool list_passive(client_t *curr_client);
bool list_active(client_t *curr_client);
void exec_list(client_t *curr_client);

bool pwd(__attribute_maybe_unused__ myftp_t *ftp_s,
    __attribute_maybe_unused__ client_t *curr_client,
    __attribute_maybe_unused__ char *subcommands);

bool pasv(__attribute_maybe_unused__ myftp_t *ftp_s,
    __attribute_maybe_unused__ client_t *curr_client,
    __attribute_maybe_unused__ char *subcommands);

bool port(__attribute_maybe_unused__ myftp_t *ftp_s,
    __attribute_maybe_unused__ client_t *curr_client,
    __attribute_maybe_unused__ char *subcommands);

bool cwd(__attribute_maybe_unused__ myftp_t *ftp_s,
    __attribute_maybe_unused__ client_t *curr_client,
    __attribute_maybe_unused__ char *subcommands);

bool cdup(__attribute_maybe_unused__ myftp_t *ftp_s,
    __attribute_maybe_unused__ client_t *curr_client,
    __attribute_maybe_unused__ char *subcommands);

bool noop(__attribute_maybe_unused__ myftp_t *ftp_s,
    __attribute_maybe_unused__ client_t *curr_client,
    __attribute_maybe_unused__ char *subcommands);

bool retr(__attribute_maybe_unused__ myftp_t *ftp_s,
    __attribute_maybe_unused__ client_t *curr_client,
    __attribute_maybe_unused__ char *subcommands);
bool retr_passive(client_t *curr_client, char *subcommands);
bool retr_active(client_t *curr_client, char *subcommands);
void exec_retr(client_t *curr_client, char *path);

bool stor(__attribute_maybe_unused__ myftp_t *ftp_s,
    __attribute_maybe_unused__ client_t *curr_client,
    __attribute_maybe_unused__ char *subcommands);
void exec_stor(client_t *curr_client, char *path);
bool stor_passive(client_t *curr_client, char *subcommands);
bool stor_active(client_t *curr_client, char *subcommands);

bool dele(__attribute_maybe_unused__ myftp_t *ftp_s,
    __attribute_maybe_unused__ client_t *curr_client,
    __attribute_maybe_unused__ char *subcommands);

// Utilities
void get_poll_array(myftp_t *ftp_s);
void set_revents(myftp_t *ftp_s);
bool can_exec_data_tranfer(client_t *curr_client);
void init_new_client(client_t *new_client, int new_client_fd,
    struct pollfd *tmp_pollfd, transfer_sock_t *transfer);
bool is_valid_command(client_t *curr_client, char *command);
bool is_file(char *path);
size_t get_file_size(char *path);
void handle_unknown_command(myftp_t *ftp_s, client_t *curr_client);

#endif /* !MYFTP_H_ */
