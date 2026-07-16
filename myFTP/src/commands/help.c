/*
** EPITECH PROJECT, 2026
** myftp
** File description:
** help
*/

/**
 * @file help.c
 * @brief Functions to execute the HELP command
 */

#include "help.h"

/**
 * @brief Display a short description of a specific command
 *
 * @param curr_client Client executing the HELP command
 * @param subcommands Command for which the client wants information
 */
static void display_command_help(client_t *curr_client, char *subcommands)
{
    for (int i = 0; i < NB_COMMANDS; i++) {
        if (strcmp(help_commands[i].command_name, subcommands) == 0) {
            dprintf(curr_client->poll_s.fd, "214 %s",
                help_commands[i].command_description);
        }
    }
}

/**
 * @brief HELP comman function
 *
 * Execution is different if arguments ar given or not.
 *
 * @param ftp_s Pointer to the main structure
 * @param curr_client Clients executing the HELP command
 * @param subcommands Potential arguments given to the PASV command
 *
 * @return Boolean value about command execution
 */
bool help(__attribute_maybe_unused__ myftp_t *ftp_s,
    __attribute_maybe_unused__ client_t *curr_client,
    __attribute_maybe_unused__ char *subcommands)
{
    const char *commands = "214 Help menu\nUSER\nPORT\nRETR\nSTOR\nDELE\n"
        "PWD\nPASS\nPASV\nLIST\nNOOP\nHELP\nQUIT\r\n";

    if (!subcommands) {
        write(curr_client->poll_s.fd, commands, strlen(commands));
        return true;
    }
    if (!is_valid_command(curr_client, subcommands)) {
        return false;
    }
    display_command_help(curr_client, subcommands);
    return true;
}
