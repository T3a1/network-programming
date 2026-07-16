/*
** EPITECH PROJECT, 2026
** G-NWP-400-LIL-4-1-myteams-2
** File description:
** UnknownTeam
*/

#include "UnknownTeam.hpp"

namespace AEL {

void UnknownTeam::handle([[maybe_unused]]const std::string& response) {
    std::istringstream iss(response);
    std::string code, team_uuid;
    iss >> code >> team_uuid;
    
    if (!team_uuid.empty())
        client_error_unknown_team(team_uuid.c_str());
}

}
