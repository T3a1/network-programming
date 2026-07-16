/*
** EPITECH PROJECT, 2026
** G-NWP-400-LIL-4-1-myteams-2
** File description:
** TeamCreated
*/

#include "TeamCreated.hpp"

namespace AEL {

void TeamCreated::handle([[maybe_unused]]const std::string& response) {
    std::istringstream iss(response);
    std::string code, team_uuid, team_name;
    iss >> code >> team_uuid;

    std::string team_description;
    std::size_t q1 = response.find('"');
    std::size_t q2 = (q1 == std::string::npos) ? std::string::npos : response.find('"', q1 + 1);
    std::size_t q3 = (q2 == std::string::npos) ? std::string::npos : response.find('"', q2 + 1);
    std::size_t q4 = (q3 == std::string::npos) ? std::string::npos : response.find('"', q3 + 1);
    if (q1 != std::string::npos && q2 != std::string::npos && q3 != std::string::npos && q4 != std::string::npos) {
        team_name = response.substr(q1 + 1, q2 - q1 - 1);
        team_description = response.substr(q3 + 1, q4 - q3 - 1);
    } else {
        iss >> team_name;
        std::getline(iss, team_description);
        if (!team_description.empty() && team_description[0] == ' ')
            team_description.erase(0, 1);
    }
    
    if (!team_uuid.empty() && !team_name.empty()) {
        std::cout << std::endl;
        client_event_team_created(team_uuid.c_str(), team_name.c_str(), team_description.c_str());
    }
    std::cout.flush();
}

}
