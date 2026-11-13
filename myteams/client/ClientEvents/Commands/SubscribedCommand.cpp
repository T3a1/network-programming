/*
** EPITECH PROJECT, 2026
** G-NWP-400-LIL-4-1-myteams-2
** File description:
** SubscribedCommand
*/

#include "SubscribedCommand.hpp"
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

bool SubscribedCommand::checkArgs(const std::string& args) {
    std::string input = trim(args);
    std::size_t quoteCount = 0;

    _uuid.clear();
    if (input.empty())
        return true;

    for (char c : input) {
        if (c == '"')
            ++quoteCount;
    }

    if (input.front() != '"' || input.back() != '"' || quoteCount != 2)
        return false;

    _uuid = input.substr(1, input.length() - 2);
    if (_uuid.empty() || _uuid.find('"') != std::string::npos)
        return false;

    return true;
}

void SubscribedCommand::handleCommand([[maybe_unused]]Client& client, [[maybe_unused]]const std::string& args) {
    if (_uuid.empty())
        client.sendCommand("SBD");
    else
        client.sendCommand("SBD " + _uuid);
}

void SubscribedCommand::readInfo(const std::string& response) {
	std::istringstream iss(response);
	std::string code;
	std::string uuid;
	std::string name;
	std::string description;

    iss >> code >> uuid;
    if (!parseTwoQuotedFields(response, name, description)) {
        iss.clear();
        iss.str(response);
        iss >> code >> uuid >> name;
        std::getline(iss, description);
        if (!description.empty() && description.front() == ' ')
            description.erase(0, 1);
    }

	if (!uuid.empty() && !name.empty() && !description.empty())
		client_print_teams(uuid.c_str(), name.c_str(), description.c_str());
}

}
