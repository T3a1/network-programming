/*
** EPITECH PROJECT, 2026
** myftp
** File description:
** help
*/

#ifndef HELP_H_
    #define HELP_H_
    #include "myftp.h"
    #define NB_COMMANDS 14

const struct ftp_help help_commands[NB_COMMANDS] = {
    {"USER", "Authenticate with a known username.\r\n"},
    {"PASS", "Enter user's password, use USER before.\r\n"},
    {"QUIT", "Logout and close connection.\r\n"},
    {"HELP", "Display implemented commands/description for each of them.\r\n"},
    {"LIST", "List current working directory content in data socket.\r\n"},
    {"PWD", "Show current working directory path from server root.\r\n"},
    {"PASV", "Turn passive mode on.\r\n"},
    {"PORT", "Turn active mode on.\r\n"},
    {"CWD", "Change working directory.\r\n"},
    {"CDUP", "Change current wordking directory to its parent.\r\n"},
    {"NOOP", "Test connection to server.\r\n"},
    {"RETR", "Get a file from the server in the data socket.\r\n"},
    {"STOR", "Store a file from client to server using data socket.\r\n"},
    {"DELE", "Delete a file stored in the server.\r\n"}
};

#endif /* !HELP_H_ */
