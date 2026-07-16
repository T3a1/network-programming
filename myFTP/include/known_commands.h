/*
** EPITECH PROJECT, 2026
** myftp
** File description:
** known_commands
*/

#ifndef KNOWN_COMMANDS_H_
    #define KNOWN_COMMANDS_H_
    #include "myftp.h"
    #define NB_COMMANDS 14

const struct ftp_commands known_command[NB_COMMANDS] = {
    {"USER", user},
    {"PASS", pass},
    {"QUIT", quit},
    {"HELP", help},
    {"LIST", list},
    {"PWD", pwd},
    {"PASV", pasv},
    {"PORT", port},
    {"CWD", cwd},
    {"CDUP", cdup},
    {"NOOP", noop},
    {"RETR", retr},
    {"STOR", stor},
    {"DELE", dele}
};

#endif /* !KNOWN_COMMANDS_H_ */
