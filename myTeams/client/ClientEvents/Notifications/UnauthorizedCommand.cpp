/*
** EPITECH PROJECT, 2026
** G-NWP-400-LIL-4-1-myteams-2
** File description:
** UnauthorizedCommand
*/

#include "UnauthorizedCommand.hpp"

namespace AEL {

void UnauthorizedCommand::handle([[maybe_unused]]const std::string& response) {
    client_error_unauthorized();
}

}
