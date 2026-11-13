/*
** EPITECH PROJECT, 2026
** G-NWP-400-LIL-4-1-myteams-2
** File description:
** SendCommand
*/

#include "SendCommand.hpp"
#include "Client.hpp"
#include <sstream>

namespace AEL {

bool SendCommand::checkArgs(const std::string& args) {
    std::stringstream ss(args);
    std::string uuid;
    std::string message;

    ss >> uuid;
    std::getline(ss, message);

    if (uuid.empty() || message.empty())
        return false;

    if (uuid.front() == '"' && uuid.back() == '"')
        uuid = uuid.substr(1, uuid.length() - 2);
    else
        return false;

    if (message.front() == ' ' && message.back() == '"' && message[1] == '"')
        message = message.substr(2, message.length() - 3);
    else
        return false;

    _uuid = uuid;
    _message = message;
    return true;
}

void SendCommand::handleCommand([[maybe_unused]]Client& client, [[maybe_unused]]const std::string& args) {
    client.sendCommand("SND " + _uuid + " " + _message);
}

void SendCommand::readInfo(const std::string& response) {
    std::istringstream iss(response);
    std::string code, user_uuid;
    iss >> code >> user_uuid;

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
    if (!user_uuid.empty() && !message_body.empty())
        std::cout << code << " Message sent to user with UUID " << user_uuid << ": " << message_body << std::endl;
}

}
