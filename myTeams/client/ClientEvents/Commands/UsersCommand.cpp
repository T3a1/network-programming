/*
** EPITECH PROJECT, 2026
** G-NWP-400-LIL-4-1-myteams-2
** File description:
** UsersCommand
*/

#include "UsersCommand.hpp"
#include <sstream>

namespace AEL {

bool UsersCommand::checkArgs(const std::string& args) {
    if (!args.empty()) {
        std::cout << "Command /users doesn't take any argument." << std::endl;
        return false;
    }
    return true;
}

void UsersCommand::handleCommand([[maybe_unused]]Client& client, [[maybe_unused]]const std::string& args) {
    client.sendCommand("USS");
}

void UsersCommand::readInfo(const std::string& response) {
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
	client_print_users(user_uuid.c_str(), user_name.c_str(), status);
}

}
