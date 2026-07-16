/*
** EPITECH PROJECT, 2026
** myftp
** File description:
** execute_commands
*/

/**
 * @file execute_commands.c
 * @brief Execute commands
 *
 * Get the command the client wants to execute, and redirects it to its funtion
 * if exist. If not, display error message about non-existing command.
 */

#include "myftp.h"
#include "known_commands.h"

/**
 * @brief Display unknown command error message
 *
 * @param ftp_s Pointer to the main structure
 * @param curr_client Client executing the command
 */
void handle_unknown_command(myftp_t *ftp_s, client_t *curr_client)
{
    dprintf(curr_client->poll_s.fd, "%s%s%s", M500_A, M500_B, M500_C);
    ftp_s->exit_status = UNKNOWN_COMMAND;
}

/**
 * @brief Checks if the command exists
 *
 * @param curr_client Client executing the command
 * @param command Command that the client wants to execute
 *
 * @return Boolean value a entered command validity
 */
bool is_valid_command(client_t *curr_client, char *command)
{
    for (int i = 0; i < NB_COMMANDS; i++) {
        if (strcmp(known_command[i].command_name, command) == 0) {
            return true;
        }
    }
    dprintf(curr_client->poll_s.fd, "%s%s%s", M500_A, M500_B, M500_C);
    return false;
}

/**
 * @brief Function that will execute the entered command
 *
 * Iterates the entered command amoung the existing ones and execute it
 * if possible
 *
 * @param ftp_s Pointer to the main structure
 * @param curr_client Client executing the command
 * @param buffer String containing the entered command and its arguments
 */
void execute_command(myftp_t *ftp_s, client_t *curr_client, char *buffer)
{
    bool valid_command = false;
    bool status = true;
    char *temp_buffer = strdup(buffer);
    char *primary_command = strtok(temp_buffer, " ");

    for (int i = 0; i < NB_COMMANDS; i++) {
        if (strcmp(known_command[i].command_name, primary_command) == 0) {
            status = known_command[i].function(
                ftp_s, curr_client, strtok(NULL, ""));
            valid_command = true;
        }
    }
    if (!valid_command)
        handle_unknown_command(ftp_s, curr_client);
    if (valid_command && status == true)
        ftp_s->exit_status = EXIT_SUCCESS;
    free(temp_buffer);
}
