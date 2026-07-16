/*
** EPITECH PROJECT, 2026
** G-NWP-400-LIL-4-1-myteams-2
** File description:
** UnknownChannel
*/

#include "UnknownChannel.hpp"

namespace AEL {

void UnknownChannel::handle([[maybe_unused]]const std::string& response) {
    std::istringstream iss(response);
    std::string code, channel_uuid;
    iss >> code >> channel_uuid;
    
    if (!channel_uuid.empty())
        client_error_unknown_channel(channel_uuid.c_str());
}

}
