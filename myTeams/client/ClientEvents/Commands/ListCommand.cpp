/*
** EPITECH PROJECT, 2026
** G-NWP-400-LIL-4-1-myteams-2
** File description:
** ListCommand
*/

#include "ListCommand.hpp"
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

bool ListCommand::checkArgs(const std::string& args) {
    if (!args.empty()) {
        std::cout << "Command /list doesn't take any argument." << std::endl;
        return false;
    }
    return true;
}

void ListCommand::handleCommand([[maybe_unused]]Client& client, [[maybe_unused]]const std::string& args) {
    client.sendCommand("LST");
}

void ListCommand::readInfo(const std::string& response) {
    std::istringstream iss(response);
    std::string code;
    iss >> code;
    std::vector<std::string> fields = extractQuotedFields(response);

    if (code == "2102") {
        std::string channel_uuid, channel_name, channel_description;
        iss >> channel_uuid;
        if (fields.size() >= 2) {
            channel_name = fields[0];
            channel_description = fields[1];
        } else {
            iss >> channel_name;
            std::getline(iss, channel_description);
            if (!channel_description.empty() && channel_description[0] == ' ')
                channel_description.erase(0, 1);
        }
        if (!channel_uuid.empty() && !channel_name.empty() && !channel_description.empty())
            client_team_print_channels(channel_uuid.c_str(), channel_name.c_str(), channel_description.c_str());
    } else if (code == "2103") {
        std::string thread_uuid, user_uuid, thread_timestamp_str, thread_title, thread_body;
        iss >> thread_uuid >> user_uuid >> thread_timestamp_str;
        if (fields.size() >= 2) {
            thread_title = fields[0];
            thread_body = fields[1];
        } else {
            iss >> thread_title;
            std::getline(iss, thread_body);
            if (!thread_body.empty() && thread_body[0] == ' ')
                thread_body.erase(0, 1);
        }
        if (!thread_uuid.empty() && !user_uuid.empty() && !thread_timestamp_str.empty() && !thread_title.empty() && !thread_body.empty()) {
            try {
                time_t timestamp = std::stol(thread_timestamp_str);
                client_channel_print_threads(thread_uuid.c_str(), user_uuid.c_str(), timestamp, thread_title.c_str(), thread_body.c_str());
            } catch (const std::invalid_argument& e) {
                std::cerr << "Error parsing thread timestamp: '" << thread_timestamp_str << "'" << std::endl;
            }
        }
    } else if (code == "2104") {
        std::string thread_uuid, user_uuid, reply_timestamp_str, reply_body;
        iss >> thread_uuid >> user_uuid >> reply_timestamp_str;
        if (!fields.empty())
            reply_body = fields[0];
        else {
            std::getline(iss, reply_body);
            if (!reply_body.empty() && reply_body[0] == ' ')
                reply_body.erase(0, 1);
        }
        if (!thread_uuid.empty() && !user_uuid.empty() && !reply_timestamp_str.empty() && !reply_body.empty()) {
            try {
                time_t timestamp = std::stol(reply_timestamp_str);
                client_thread_print_replies(thread_uuid.c_str(), user_uuid.c_str(), timestamp, reply_body.c_str());
            } catch (const std::invalid_argument& e) {
                std::cerr << "Error parsing reply timestamp: '" << reply_timestamp_str << "'" << std::endl;
            }
        }
    }
}

}
