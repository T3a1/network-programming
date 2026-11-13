/*
** EPITECH PROJECT, 2026
** G-NWP-400-LIL-4-1-myteams-2
** File description:
** PrivateMessage
*/

#include "PrivateMessage.hpp"

namespace AEL {

void PrivateMessage::handle([[maybe_unused]]const std::string& response) {
    std::istringstream iss(response);
    std::string code, sender_uuid;
    iss >> code >> sender_uuid;
    
    std::string message_body;
    std::size_t firstQuote = response.find('"');
    std::size_t lastQuote = response.find_last_of('"');
    if (firstQuote != std::string::npos && lastQuote != std::string::npos && lastQuote > firstQuote)
        message_body = response.substr(firstQuote + 1, lastQuote - firstQuote - 1);
    else {
        std::getline(iss, message_body);
        if (!message_body.empty() && message_body[0] == ' ')
            message_body.erase(0, 1);
    }
    
    if (!sender_uuid.empty() && !message_body.empty()) {
        std::cout << std::endl;
        client_event_private_message_received(sender_uuid.c_str(), message_body.c_str());
    }
    std::cout.flush();
}

}
