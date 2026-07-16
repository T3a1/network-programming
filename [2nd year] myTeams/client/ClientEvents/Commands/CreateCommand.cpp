/*
** EPITECH PROJECT, 2026
** G-NWP-400-LIL-4-1-myteams-2
** File description:
** CreateCommand
*/

#include "CreateCommand.hpp"
#include "Client.hpp"
#include <sstream>
#include <functional>
#include <map>
#include <vector>

namespace AEL {

static std::string trim(const std::string &value)
{
    std::size_t start = value.find_first_not_of(" \t\n\r");
    if (start == std::string::npos)
        return "";
    std::size_t end = value.find_last_not_of(" \t\n\r");
    return value.substr(start, end - start + 1);
}

static bool parseTwoQuotedFields(const std::string &input, std::string &first, std::string &second)
{
    std::size_t q1 = input.find('"');
    if (q1 == std::string::npos)
        return false;
    std::size_t q2 = input.find('"', q1 + 1);
    if (q2 == std::string::npos)
        return false;
    std::size_t q3 = input.find('"', q2 + 1);
    if (q3 == std::string::npos)
        return false;
    std::size_t q4 = input.find('"', q3 + 1);
    if (q4 == std::string::npos)
        return false;

    first = input.substr(q1 + 1, q2 - q1 - 1);
    second = input.substr(q3 + 1, q4 - q3 - 1);
    return true;
}

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

bool CreateCommand::checkArgs(const std::string& args) {
    _args.clear();
    std::string input = trim(args);
    std::size_t pos = 0;

    if (input.empty())
        return false;

    while (pos < input.size()) {
        while (pos < input.size() && input[pos] == ' ')
            ++pos;
        if (pos >= input.size())
            break;
        if (input[pos] != '"')
            return false;
        std::size_t end = input.find('"', pos + 1);
        if (end == std::string::npos || end == pos + 1)
            return false;
        _args.push_back(input.substr(pos + 1, end - pos - 1));
        pos = end + 1;
        while (pos < input.size() && input[pos] == ' ')
            ++pos;
    }

    if (_args.size() < 1 || _args.size() > 2)
        return false;

    return true;
}

void CreateCommand::handleCommand([[maybe_unused]]Client& client, [[maybe_unused]]const std::string& args) {
    int contextLevel = client.getContextLevel();

    auto getCreateType = [](int level) -> std::string {
        const std::map<int, std::string> commandMap = {
            {0, "CRT"},
            {1, "CRC"},
            {2, "CRH"},
            {3, "CRR"}
        };
        auto it = commandMap.find(level);
        if (it != commandMap.end())
            return it->second;
        return "";
    };

    std::string command = getCreateType(contextLevel);
    if (command.empty())
        return;

    for (const auto& arg : _args)
        command += " \"" + arg + "\"";
    client.sendCommand(command);
}

void CreateCommand::readInfo(const std::string& response) {
    std::istringstream iss(response);
    std::string code;
    iss >> code;
    if (code == "2300") {
        std::string team_uuid;
        std::string team_name;
        std::string team_desc;
        iss >> team_uuid;
        if (!parseTwoQuotedFields(response, team_name, team_desc)) {
            iss.clear();
            iss.str(response);
            iss >> code >> team_uuid >> team_name;
            std::getline(iss, team_desc);
            if (!team_desc.empty() && team_desc[0] == ' ')
                team_desc.erase(0, 1);
        }
        client_print_team_created(team_uuid.c_str(), team_name.c_str(), team_desc.c_str());
    } else if (code == "2301") {
        std::string channel_uuid, channel_name;
        iss >> channel_uuid;
        std::string channel_desc;
        std::vector<std::string> fields = extractQuotedFields(response);
        if (fields.size() >= 2) {
            channel_name = fields[0];
            channel_desc = fields[1];
        } else {
            iss >> channel_name;
            std::getline(iss, channel_desc);
            if (!channel_desc.empty() && channel_desc[0] == ' ')
                channel_desc.erase(0, 1);
        }
        client_print_channel_created(channel_uuid.c_str(), channel_name.c_str(), channel_desc.c_str());
    } else if (code == "2302") {
        std::string thread_uuid, user_uuid, thread_timestamp_str, thread_title;
        iss >> thread_uuid >> user_uuid >> thread_timestamp_str;
        std::string thread_body;
        std::vector<std::string> fields = extractQuotedFields(response);
        if (fields.size() >= 2) {
            thread_title = fields[0];
            thread_body = fields[1];
        } else {
            iss >> thread_title;
            std::getline(iss, thread_body);
            if (!thread_body.empty() && thread_body[0] == ' ')
                thread_body.erase(0, 1);
        }
        time_t thread_timestamp = std::stol(thread_timestamp_str);
        client_print_thread_created(thread_uuid.c_str(), user_uuid.c_str(), thread_timestamp, thread_title.c_str(), thread_body.c_str());
    } else if (code == "2303") {
        std::string thread_uuid, user_uuid, reply_timestamp_str;
        iss >> thread_uuid >> user_uuid >> reply_timestamp_str;
        std::string reply_body;
        std::vector<std::string> fields = extractQuotedFields(response);
        if (!fields.empty())
            reply_body = fields[0];
        else {
            std::getline(iss, reply_body);
            if (!reply_body.empty() && reply_body[0] == ' ')
                reply_body.erase(0, 1);
        }
        time_t reply_timestamp = std::stol(reply_timestamp_str);
        client_print_reply_created(thread_uuid.c_str(), user_uuid.c_str(), reply_timestamp, reply_body.c_str());
    }
}

}
