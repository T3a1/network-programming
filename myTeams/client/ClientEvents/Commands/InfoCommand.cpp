/*
** EPITECH PROJECT, 2026
** G-NWP-400-LIL-4-1-myteams-2
** File description:
** InfoCommand
*/

#include "InfoCommand.hpp"
#include "Client.hpp"
#include <iostream>
#include <functional>
#include <sstream>
#include <map>
#include <vector>

namespace AEL {

static std::vector<std::string> extractQuotedFields(const std::string &input)
{
    std::vector<std::string> fields;
    std::size_t pos = 0;

    while (true) {
        std::size_t q1 = input.find('"', pos);
        if (q1 == std::string::npos)
            break;
        std::size_t q2 = input.find('"', q1 + 1);
        if (q2 == std::string::npos)
            break;
        fields.push_back(input.substr(q1 + 1, q2 - q1 - 1));
        pos = q2 + 1;
    }
    return fields;
}

bool InfoCommand::checkArgs(const std::string& args) {
    if (!args.empty()) {
        std::cout << "Command /info doesn't take any argument." << std::endl;
        return false;
    }
    return true;
}

void InfoCommand::handleCommand([[maybe_unused]]Client& client, [[maybe_unused]]const std::string& args) {
    int contextLevel = client.getContextLevel();

    auto getCommandPrefix = [](int level) -> std::string {
        const std::map<int, std::string> commandMap = {
            {0, "IFU"},
            {1, "IFT"},
            {2, "IFC"},
            {3, "IFH"}
        };
        auto it = commandMap.find(level);
        if (it != commandMap.end())
            return it->second;
        return "";
    };

    std::string command = getCommandPrefix(contextLevel);
    if (command.empty())
        return;

    client.sendCommand(command);
}

void InfoCommand::readInfo(const std::string& response) {
    std::istringstream iss(response);
    std::string code;
    iss >> code;
    std::vector<std::string> fields = extractQuotedFields(response);

    if (code == "2201") {
        std::string uuid, name;
        std::string description;
        iss >> uuid;
        if (fields.size() >= 2) {
            name = fields[0];
            description = fields[1];
        } else {
            iss >> name;
            std::getline(iss, description);
            if (!description.empty() && description[0] == ' ')
                description = description.substr(1);
        }
        if (!uuid.empty() && !name.empty())
            client_print_team(uuid.c_str(), name.c_str(), description.c_str());
        return;
    }

    if (code == "2202") {
        std::string uuid, name;
        std::string description;
        iss >> uuid;
        if (fields.size() >= 2) {
            name = fields[0];
            description = fields[1];
        } else {
            iss >> name;
            std::getline(iss, description);
            if (!description.empty() && description[0] == ' ')
                description = description.substr(1);
        }
        if (!uuid.empty() && !name.empty())
            client_print_channel(uuid.c_str(), name.c_str(), description.c_str());
        return;
    }

    if (code == "2203") {
        std::string uuid, author_uuid, timestamp_str, title;
        std::string body;
        iss >> uuid >> author_uuid >> timestamp_str;
        if (fields.size() >= 2) {
            title = fields[0];
            body = fields[1];
        } else {
            iss >> title;
            std::getline(iss, body);
            if (!body.empty() && body[0] == ' ')
                body = body.substr(1);
        }
        if (!uuid.empty() && !author_uuid.empty() && !timestamp_str.empty() && !title.empty()) {
            time_t timestamp = static_cast<time_t>(std::stol(timestamp_str));
            client_print_thread(uuid.c_str(), author_uuid.c_str(), timestamp, title.c_str(), body.c_str());
        }
        return;
    }
}

}
