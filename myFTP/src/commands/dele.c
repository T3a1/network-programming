/*
** EPITECH PROJECT, 2026
** myftp
** File description:
** dele
*/

/**
 * @file dele.c
 * @brief Function to execute the DELE command
 */

#include "myftp.h"

/**
 * @brief DELE command function
 *
 * @param ftp_s Pointer to the main structure
 * @param curr_client Clients executing the DELE command
 * @param subcommands Potential arguments given to the DELE command
 *
 * @return Boolean value about command execution.
 */
bool dele(__attribute_maybe_unused__ myftp_t *ftp_s,
    __attribute_maybe_unused__ client_t *curr_client,
    __attribute_maybe_unused__ char *subcommands)
{
    if (!curr_client->verified) {
        dprintf(curr_client->poll_s.fd, "%s", M530);
        return false;
    }
    if (!is_file(subcommands)) {
        dprintf(curr_client->poll_s.fd, "%s", M550);
        return false;
    }
    remove(subcommands);
    dprintf(curr_client->poll_s.fd, "%s", M250);
    return true;
}
