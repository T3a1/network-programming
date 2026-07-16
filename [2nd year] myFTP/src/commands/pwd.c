/*
** EPITECH PROJECT, 2026
** myftp
** File description:
** pwd
*/

/**
 * @file pwd.c
 * @brief File containing the PWD command function
 */

#include "myftp.h"

/**
 * @brief PWD command function
 *
 * @param ftp_s Pointer to the main structure
 * @param curr_client Clients executing the QUIT command
 * @param subcommands Potential arguments given to the QUIT command
 *
 * @return Boolean value about command execution
 */
bool pwd(__attribute_maybe_unused__ myftp_t *ftp_s,
    __attribute_maybe_unused__ client_t *curr_client,
    __attribute_maybe_unused__ char *subcommands)
{
    if (!curr_client->verified) {
        write(curr_client->poll_s.fd, M530, strlen(M530));
        return false;
    }
    dprintf(curr_client->poll_s.fd, "257 \"%s\" created.\r\n",
        ftp_s->working_dir);
    return true;
}
