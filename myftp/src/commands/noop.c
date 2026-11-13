/*
** EPITECH PROJECT, 2026
** myftp
** File description:
** noop
*/

/**
 * @file noop.c
 * @brief Function to exexute the NOOP command
 */

#include "myftp.h"

/**
 * @brief NOOP command function
 *
 * @param ftp_s Pointer to the main structure
 * @param curr_client Clients executing the NOOP command
 * @param subcommands Potential arguments given to the NOOP command
 *
 * @return Boolean value about command execution. Always true in this context.
 */
bool noop(__attribute_maybe_unused__ myftp_t *ftp_s,
    __attribute_maybe_unused__ client_t *curr_client,
    __attribute_maybe_unused__ char *subcommands)
{
    dprintf(curr_client->poll_s.fd, M200);
    return true;
}
