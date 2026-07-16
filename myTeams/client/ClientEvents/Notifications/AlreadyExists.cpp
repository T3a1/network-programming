/*
** EPITECH PROJECT, 2026
** G-NWP-400-LIL-4-1-myteams-2
** File description:
** AlreadyExists
*/

#include "AlreadyExists.hpp"

namespace AEL {

void AlreadyExists::handle([[maybe_unused]]const std::string& response) {
    client_error_already_exist();
}

}
