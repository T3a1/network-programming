/*
** EPITECH PROJECT, 2026
** myftp
** File description:
** main
*/

/**
 * @file main.c
 * @brief File containing the main function. Server starting point.
 *
 * Contains the main function, flag detection, and initialization functions.
 */

#include "myftp.h"

/**
 * @brief Help flag
 *
 * Check and diplay help
 * @param argv Array containing the cli arguments
 * @param flag Pointer to boolean to inform if help flag is triggered
 *
 * @return Flag trigger
 */
static bool help_flag(char **argv, bool *flag)
{
    for (int i = 0; argv[i]; i++) {
        if (strcmp(argv[i], "-h") == 0 || strcmp(argv[i], "--help") == 0) {
            *flag = true;
            break;
        }
    }
    if (!*flag)
        return false;
    printf("Usage: myftp [PORT] [ROOT_DIRECTORY]\n"
        "    myftp is a basic reimplementation of a ftp server.\n"
        "    Launch server and run client-side HELP command to see available "
        "commands.\n    Run HELP [COMMAND] to see a short description of the "
        "selected command.\n");
    return true;
}

/**
 * @brief Second 'Initialization' function
 *
 * Initialize attributes in the main structure.
 * Start server at its defined root.
 *
 * @param ftp_s Pointer to the main structure.
 * @param path Root of the FTP sever.
 */
static void initialize_ftp_s2(myftp_t *ftp_s, char *path)
{
    for (int i = 0; i < MAX_CONNECTIONS; i++)
        ftp_s->poll_arr[i].fd = -1;
    ftp_s->working_dir = strdup("/");
    chdir(path);
    getcwd(ftp_s->absolute_root, MAX_READ);
}

/**
 * @brief First 'Initialization' function
 *
 * Initialize attributes in the main structure
 *
 * @param ftp_s Pointer to the main structure
 * @param path Root of the FTP server
 */
void initialize_ftp_s1(myftp_t *ftp_s, char *path)
{
    ftp_s->control_fd = -1;
    ftp_s->nb_clients = 0;
    ftp_s->exit_status = EXIT_SUCCESS;
    ftp_s->clients = calloc(1, sizeof(client_t *));
    ftp_s->absolute_root = calloc(MAX_READ, sizeof(char));
    if (!ftp_s->clients || !ftp_s->absolute_root) {
        ftp_s->exit_status = EXIT_FAILURE;
        return;
    }
    *ftp_s->clients = NULL;
    ftp_s->poll_arr = calloc(MAX_CONNECTIONS, sizeof(struct pollfd));
    if (!ftp_s->poll_arr) {
        free(ftp_s->clients);
        ftp_s->exit_status = EXIT_FAILURE;
        return;
    }
    initialize_ftp_s2(ftp_s, path);
}

/**
 * @brief Deinilize all structures
 *
 * Properly free all remaining allocated memory.
 * Also close open file descriptors.
 *
 * @param ftp_s Pointer to main structure
 */
void deinitialize_ftp_s(myftp_t *ftp_s)
{
    free(ftp_s->poll_arr);
    close(ftp_s->control_fd);
    free(ftp_s->clients);
    free(ftp_s->working_dir);
    free(ftp_s->absolute_root);
}

/**
 * @brief Call initialization functions
 *
 * Create initialization structures with all defulats informations
 * needed by the ftp server to handle all events properly.
 *
 * @param ftp_s Pointer to main strucure.
 * @param argv Array containng all cli structures.
 */
static void initialize_all(myftp_t *ftp_s, char **argv)
{
    initialize_ftp_s1(ftp_s, argv[2]);
    if (ftp_s->exit_status == EXIT_FAILURE) {
        fprintf(stderr, "Initialization failed\n");
        return;
    }
    initialize_server(ftp_s, atoi(argv[1]));
    if (ftp_s->exit_status == EXIT_FAILURE) {
        fprintf(stderr, "Server initialization failed\n");
        deinitialize_ftp_s(ftp_s);
    }
}

/**
 * @brief Main function
 *
 * Check cli arguments.
 * Initialize nescessary structures for the server to work.
 * Start clients handling.
 *
 * @param argc Number of cli args.
 * @param argv Array containing cli args.
 * @return Exit status
 */
int main(int argc, char **argv)
{
    myftp_t ftp_s;
    bool flag = false;

    if (help_flag(argv, &flag))
        return EXIT_SUCCESS;
    if (argc < 3 || (atoi(argv[1]) == 0) || (access(argv[2], F_OK) == -1)) {
        flag = true;
        help_flag(argv, &flag);
        return EXIT_FAILURE;
    }
    initialize_all(&ftp_s, argv);
    if (ftp_s.exit_status == EXIT_FAILURE)
        return EXIT_FAILURE;
    handle_clients(&ftp_s);
    deinitialize_ftp_s(&ftp_s);
    return (ftp_s.exit_status == UNKNOWN_COMMAND) ? EXIT_FAILURE :
        ftp_s.exit_status;
}
