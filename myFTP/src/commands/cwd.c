/*
** EPITECH PROJECT, 2026
** myftp
** File description:
** cwd
*/

/**
 * @file cwd.c
 * @brief Functions to change working directory inside the FTP server
 */

#include "myftp.h"

/**
 * @brief Checks if entered path is an existing directory
 *
 * @param path Path to the new directory
 *
 * @return Boolean value about path status
 */
static bool is_dir(const char *path)
{
    struct stat sb;

    return lstat(path, &sb) == 0 && S_ISDIR(sb.st_mode);
}

/**
 * @brief Checks if directory change is possible
 *
 * @param client Client executing the CWD command
 * @param path New entered path
 *
 * @return Boolean value about execution possibility
 */
static bool can_cwd(client_t *client, char *path)
{
    if (!client->verified) {
        dprintf(client->poll_s.fd, "%s", M530);
        return false;
    }
    if (!path) {
        dprintf(client->poll_s.fd, "%s", M501);
        return false;
    }
    return true;
}

/**
 * @brief Change directory to new path
 *
 * @param ftp_s Pointer to the main structure
 * @param curr_client Client executing the CWD command
 * @param full_path Absolute path to the new location
 * @param subcommands Relative path to the new location
 *
 * @return Boolean value about changing directory status
 */
static bool change_dir(myftp_t *ftp_s, client_t *curr_client, char *full_path,
    char *subcommands)
{
    strcpy(full_path, ftp_s->absolute_root);
    strcat(full_path, ftp_s->working_dir);
    strcat(full_path, subcommands);
    if (!is_dir(full_path)) {
        dprintf(curr_client->poll_s.fd, "%s", M550);
        return false;
    }
    if (chdir(full_path) == -1) {
        dprintf(curr_client->poll_s.fd, "%s", M550);
        return false;
    }
    return true;
}

/**
 * @brief CWD command function
 *
 * @param ftp_s Pointer to the main structure
 * @param curr_client Clients executing the CWD command
 * @param subcommands Potential arguments given to the CWD command
 *
 * @return Boolean value about command execution.
 */
bool cwd(__attribute_maybe_unused__ myftp_t *ftp_s,
    __attribute_maybe_unused__ client_t *curr_client,
    __attribute_maybe_unused__ char *subcommands)
{
    char full_path[MAX_READ] = {0};
    size_t new_len = 0;

    if (!can_cwd(curr_client, subcommands))
        return false;
    new_len = strlen(ftp_s->working_dir) + strlen(subcommands) + 2;
    if (!change_dir(ftp_s, curr_client, full_path, subcommands))
        return false;
    ftp_s->working_dir = realloc(ftp_s->working_dir, new_len);
    if (!ftp_s->working_dir)
        return false;
    strcat(ftp_s->working_dir, subcommands);
    strcat(ftp_s->working_dir, "/");
    dprintf(curr_client->poll_s.fd, M250);
    return true;
}
