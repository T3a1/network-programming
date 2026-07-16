/*
** EPITECH PROJECT, 2026
** G-NWP-400-LIL-4-1-myteams-2
** File description:
** SubscribeCommand
*/

#include "SubscribeCommand.hpp"
#include <sstream>
#include <iostream>

namespace AEL {

static std::string trim(const std::string &value)
{
    std::size_t start = value.find_first_not_of(" \t\n\r");
    if (start == std::string::npos)
        return "";
    std::size_t end = value.find_last_not_of(" \t\n\r");
    return value.substr(start, end - start + 1);
}

bool SubscribeCommand::checkArgs(const std::string& args) {
    std::string uuid = trim(args);
    std::size_t quoteCount = 0;

    for (char c : uuid) {
        if (c == '"')
            ++quoteCount;
    }

    if (uuid.empty())
        return false;

    if (uuid.front() != '"' || uuid.back() != '"' || quoteCount != 2)
        return false;

    _uuid = uuid.substr(1, uuid.length() - 2);
    if (_uuid.empty() || _uuid.find('"') != std::string::npos)
        return false;

    return true;
}

void SubscribeCommand::handleCommand([[maybe_unused]]Client& client, [[maybe_unused]]const std::string& args) {
    client.sendCommand("SUB " + _uuid);
}

void SubscribeCommand::readInfo(const std::string& response) {
    std::istringstream iss(response);
    std::string code;
    iss >> code;

    std::string user_uuid, team_uuid;
    iss >> user_uuid >> team_uuid;
    client_print_subscribed(user_uuid.c_str(), team_uuid.c_str());
}

}
