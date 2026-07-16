/*
** EPITECH PROJECT, 2026
** G-NWP-400-LIL-4-1-myteams-2
** File description:
** ThreadReply
*/

#include "ThreadReply.hpp"

namespace AEL {

void ThreadReply::handle([[maybe_unused]]const std::string& response) {
    std::istringstream iss(response);
    std::string code, team_uuid, thread_uuid, user_uuid;
    iss >> code >> team_uuid >> thread_uuid >> user_uuid;
    
    std::string reply_body;
    std::size_t firstQuote = response.find('"');
    std::size_t lastQuote = response.find_last_of('"');
    if (firstQuote != std::string::npos && lastQuote != std::string::npos && lastQuote > firstQuote)
        reply_body = response.substr(firstQuote + 1, lastQuote - firstQuote - 1);
    else {
        std::getline(iss, reply_body);
        if (!reply_body.empty() && reply_body[0] == ' ')
            reply_body.erase(0, 1);
    }
    
    if (!team_uuid.empty() && !thread_uuid.empty() && !user_uuid.empty() && !reply_body.empty()) {
        std::cout << std::endl;
        client_event_thread_reply_received(team_uuid.c_str(), thread_uuid.c_str(), user_uuid.c_str(), reply_body.c_str());
    }
    std::cout.flush();
}

}
