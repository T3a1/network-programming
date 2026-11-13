/*
** EPITECH PROJECT, 2026
** myftp
** File description:
** pass
*/

/**
 * @file pass.c
 * @brief Function to execute the PASS command
 */

#include "myftp.h"

/**
 * @brief PASS command function
 *
 * @param ftp_s Pointer to the main structure
 * @param curr_client Clients executing the PASS command
 * @param subcommands Potential arguments given to the PASS command
 *
 * @return Boolean value about command execution
 */
bool pass(__attribute_maybe_unused__ myftp_t *ftp_s,
    __attribute_maybe_unused__ client_t *curr_client,
    __attribute_maybe_unused__ char *subcommands)
{
    if (!curr_client->username) {
        write(curr_client->poll_s.fd, M332, strlen(M332));
        return false;
    }
    if (strcmp(curr_client->username, "Anonymous") == 0
        && !subcommands) {
        write(curr_client->poll_s.fd, M230, strlen(M230));
        curr_client->verified = true;
    } else {
        write(curr_client->poll_s.fd, M530, strlen(M530));
        return false;
    }
    return true;
}
