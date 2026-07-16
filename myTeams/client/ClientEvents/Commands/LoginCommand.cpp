/*
** EPITECH PROJECT, 2026
** G-NWP-400-LIL-4-1-myteams-2
** File description:
** LoginCommand
*/

#include "LoginCommand.hpp"
#include <sstream>

namespace AEL {

static std::string trim(const std::string &value)
{
    std::size_t start = value.find_first_not_of(" \t\n\r");
    if (start == std::string::npos)
        return "";
    std::size_t end = value.find_last_not_of(" \t\n\r");
    return value.substr(start, end - start + 1);
}

bool LoginCommand::checkArgs(const std::string& args) {
    std::string username = trim(args);
    std::size_t quoteCount = 0;

    for (char c : username) {
        if (c == '"')
            ++quoteCount;
    }

    if (username.empty())
        return false;

    if (username.front() != '"' || username.back() != '"' || quoteCount != 2)
        return false;

    _username = username.substr(1, username.size() - 2);
    return !_username.empty() && _username.find('"') == std::string::npos;
}

void LoginCommand::handleCommand([[maybe_unused]]Client& client, [[maybe_unused]]const std::string& args) {
    (void)args;
    client.sendCommand("LGI \"" + _username + "\"");
}

void LoginCommand::readInfo(const std::string& response) {
    std::istringstream iss(response);
    std::string code, user_uuid, user_name;

    iss >> code >> user_uuid;
    std::size_t firstQuote = response.find('"');
    std::size_t lastQuote = response.find_last_of('"');

    if (firstQuote != std::string::npos && lastQuote != std::string::npos && lastQuote > firstQuote)
        user_name = response.substr(firstQuote + 1, lastQuote - firstQuote - 1);
    else {
        std::getline(iss, user_name);
        if (!user_name.empty() && user_name.front() == ' ')
            user_name.erase(0, 1);
    }
    if (!user_uuid.empty() && !user_name.empty())
        client_event_logged_in(user_uuid.c_str(), user_name.c_str());
}

}
