/*
** EPITECH PROJECT, 2026
** myftp
** File description:
** user
*/

/**
 * @file user.c
 * @brief Contains the function to execute the USER command.
 */

#include "myftp.h"


/**
 * @brief USER command function
 *
 * @param ftp_s Pointer to the main structure
 * @param curr_client Client executing the USER command
 * @param subcommands Potential arguments given to the USER command
 *
 * @return Boolean value about command execution
 */
bool user(__attribute_maybe_unused__ myftp_t *ftp_s,
    __attribute_maybe_unused__ client_t *curr_client,
    __attribute_maybe_unused__ char *subcommands)
{
    if (!subcommands || *subcommands == '\0') {
        write(curr_client->poll_s.fd, M530, strlen(M530));
        return false;
    }
    write(curr_client->poll_s.fd, M331, strlen(M331));
    if (curr_client->username) {
        free(curr_client->username);
        curr_client->username = NULL;
    }
    curr_client->username = strdup(subcommands);
    return true;
}
