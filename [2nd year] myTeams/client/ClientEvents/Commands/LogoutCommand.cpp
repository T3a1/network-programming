/*
** EPITECH PROJECT, 2026
** G-NWP-400-LIL-4-1-myteams-2
** File description:
** LogoutCommand
*/

#include "LogoutCommand.hpp"
#include "Client.hpp"
#include <sstream>

namespace AEL {

bool LogoutCommand::checkArgs(const std::string& args) {
    if (!args.empty()) {
        std::cout << "Command /logout doesn't take any argument." << std::endl;
        return false;
    }
    return true;
}

void LogoutCommand::handleCommand([[maybe_unused]]Client& client, [[maybe_unused]]const std::string& args) {
    client.sendCommand("LGO");
}

void LogoutCommand::readInfo(const std::string& response) {
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
        client_event_logged_out(user_uuid.c_str(), user_name.c_str());
}

}
