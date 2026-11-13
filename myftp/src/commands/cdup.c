/*
** EPITECH PROJECT, 2026
** myftp
** File description:
** cdup
*/

/**
 * @file cdup.c
 * @brief Functions to execute the CDUP command
 */

#include "myftp.h"

/**
 * @brief Executes chdir syscall and modifies server working dir informations
 *
 * @param ftp_s Pointer to the main structure
 */
static void my_chdir(myftp_t *ftp_s)
{
    char tmp_path[MAX_READ] = {0};

    memset(tmp_path, 0, MAX_READ);
    strcat(tmp_path, ftp_s->absolute_root);
    strcat(tmp_path, ftp_s->working_dir);
    chdir(tmp_path);
}

/**
 * @brief Checks if CDUP execution is possible and meaningfull
 *
 * @param ftp_s Pointer to the main structure
 * @param curr_client Client executing the CDUP command
 *
 * @return Boolean value about execution possibility and meaning
 */
static bool can_cdup(myftp_t *ftp_s, client_t *curr_client)
{
    if (!curr_client->verified) {
        write(curr_client->poll_s.fd, M550, strlen(M550));
        return false;
    }
    if (strcmp(ftp_s->working_dir, "/") == 0) {
        dprintf(curr_client->poll_s.fd, "%s", M501);
        return false;
    }
    return true;
}

/**
 * @brief CDUP command function
 *
 * @param ftp_s Pointer to the main structure
 * @param curr_client Clients executing the CDUP command
 * @param subcommands Potential arguments given to the CDUP command
 *
 * @return Boolean value about command execution.
 */
bool cdup(__attribute_maybe_unused__ myftp_t *ftp_s,
    __attribute_maybe_unused__ client_t *curr_client,
    __attribute_maybe_unused__ char *subcommands)
{
    size_t index = 0;
    char *new_path = NULL;

    if (!can_cdup(ftp_s, curr_client))
        return false;
    index = strlen(ftp_s->working_dir) - 2;
    while (index > 0 && ftp_s->working_dir[index] != '/')
        index--;
    new_path = strndup(ftp_s->working_dir, index + 1);
    if (!new_path)
        return false;
    free(ftp_s->working_dir);
    ftp_s->working_dir = new_path;
    my_chdir(ftp_s);
    dprintf(curr_client->poll_s.fd, "%s", M200);
    return true;
}
