/*
** EPITECH PROJECT, 2026
** G-NWP-400-LIL-4-1-myteams-2
** File description:
** UnknownUser
*/

#include "UnknownUser.hpp"

namespace AEL {

void UnknownUser::handle([[maybe_unused]]const std::string& response) {
    std::istringstream iss(response);
    std::string code, user_uuid;
    iss >> code >> user_uuid;
    
    if (!user_uuid.empty())
        client_error_unknown_user(user_uuid.c_str());
}

}
