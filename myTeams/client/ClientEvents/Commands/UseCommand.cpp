/*
** EPITECH PROJECT, 2026
** G-NWP-400-LIL-4-1-myteams-2
** File description:
** UseCommand
*/

#include "UseCommand.hpp"
#include "Client.hpp"
#include <sstream>

namespace AEL {

bool UseCommand::checkArgs(const std::string& args) {
    _uuids.clear();
    
    if (args.empty())
        return true;

    std::stringstream ss(args);
    std::string uuid;
    int count = 0;

    while (ss >> uuid) {
        if (uuid.front() == '"' && uuid.back() == '"') {
            _uuids.push_back(uuid.substr(1, uuid.length() - 2));
            count++;
        } else
            return false;
    }

    if (count > 3)
        return false;
    return true;
}

void UseCommand::handleCommand([[maybe_unused]]Client& client, [[maybe_unused]]const std::string& args) {
    client.setContext(_uuids);
    
    std::string command = "USE";
    for (const auto& uuid : _uuids)
        command += " " + uuid;
    client.sendCommand(command);
}

void UseCommand::readInfo(const std::string& response) {
    std::istringstream iss(response);
    std::string code, team_uuid, channel_uuid, thread_uuid;
    iss >> code >> team_uuid >> channel_uuid >> thread_uuid;

    std::cout << code << " Context changed to: Team UUID=" << team_uuid
        << ", Channel UUID=" << channel_uuid
        << ", Thread UUID=" << thread_uuid << std::endl;
}

}
