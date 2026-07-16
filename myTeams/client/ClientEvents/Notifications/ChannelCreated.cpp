/*
** EPITECH PROJECT, 2026
** G-NWP-400-LIL-4-1-myteams-2
** File description:
** ChannelCreated
*/

#include "ChannelCreated.hpp"

namespace AEL {

void ChannelCreated::handle([[maybe_unused]]const std::string& response) {
    std::istringstream iss(response);
    std::string code, channel_uuid, channel_name;
    iss >> code >> channel_uuid;

    std::string channel_description;
    std::size_t q1 = response.find('"');
    std::size_t q2 = (q1 == std::string::npos) ? std::string::npos : response.find('"', q1 + 1);
    std::size_t q3 = (q2 == std::string::npos) ? std::string::npos : response.find('"', q2 + 1);
    std::size_t q4 = (q3 == std::string::npos) ? std::string::npos : response.find('"', q3 + 1);
    if (q1 != std::string::npos && q2 != std::string::npos && q3 != std::string::npos && q4 != std::string::npos) {
        channel_name = response.substr(q1 + 1, q2 - q1 - 1);
        channel_description = response.substr(q3 + 1, q4 - q3 - 1);
    } else {
        iss >> channel_name;
        std::getline(iss, channel_description);
        if (!channel_description.empty() && channel_description[0] == ' ')
            channel_description.erase(0, 1);
    }
    
    if (!channel_uuid.empty() && !channel_name.empty()) {
        std::cout << std::endl;
        client_event_channel_created(channel_uuid.c_str(), channel_name.c_str(), channel_description.c_str());
    }
    std::cout.flush();
}

}
