/*
** EPITECH PROJECT, 2026
** G-NWP-400-LIL-4-1-myteams-2
** File description:
** UserCommand
*/

#include "UserCommand.hpp"
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

bool UserCommand::checkArgs(const std::string& args) {
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

void UserCommand::handleCommand([[maybe_unused]]Client& client, [[maybe_unused]]const std::string& args) {
    client.sendCommand("USR " + _uuid);
}

void UserCommand::readInfo(const std::string& response) {
	std::istringstream iss(response);
	std::string code;
	std::string user_uuid;

    iss >> code >> user_uuid;
    if (code.empty() || user_uuid.empty())
        return;

    std::string user_name;
    std::string user_status;
    std::size_t firstQuote = response.find('"');
    std::size_t lastQuote = response.find_last_of('"');
    if (firstQuote != std::string::npos && lastQuote != std::string::npos && lastQuote > firstQuote) {
        user_name = response.substr(firstQuote + 1, lastQuote - firstQuote - 1);
        std::string trailing = response.substr(lastQuote + 1);
        std::istringstream tss(trailing);
        tss >> user_status;
    } else {
        std::string remaining;
        std::getline(iss, remaining);
        if (!remaining.empty() && remaining.front() == ' ')
            remaining.erase(0, 1);

        std::size_t lastSpace = remaining.find_last_of(' ');
        if (lastSpace == std::string::npos)
            return;
        user_name = remaining.substr(0, lastSpace);
        user_status = remaining.substr(lastSpace + 1);
    }
    if (user_name.empty() || user_status.empty())
        return;
    int status = std::stoi(user_status);
    client_print_user(user_uuid.c_str(), user_name.c_str(), status);
}

}
