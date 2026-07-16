/*
** EPITECH PROJECT, 2026
** G-NWP-400-LIL-4-1-myteams-2
** File description:
** MessagesCommand
*/

#include "MessagesCommand.hpp"
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

bool MessagesCommand::checkArgs(const std::string& args) {
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

void MessagesCommand::handleCommand([[maybe_unused]]Client& client, [[maybe_unused]]const std::string& args) {
    client.sendCommand("MSG " + _uuid);
}

void MessagesCommand::readInfo(const std::string& response) {
	std::istringstream iss(response);
	std::string code;
	std::string sender_uuid;
	std::string timestamp_str;
	std::string message_body;

	iss >> code >> sender_uuid >> timestamp_str;
	
	std::size_t firstQuote = response.find('"');
	std::size_t lastQuote = response.find_last_of('"');
	if (firstQuote != std::string::npos && lastQuote != std::string::npos && lastQuote > firstQuote)
		message_body = response.substr(firstQuote + 1, lastQuote - firstQuote - 1);
	else {
		std::getline(iss, message_body);
		if (!message_body.empty() && message_body[0] == ' ')
			message_body = message_body.substr(1);
	}

	if (!sender_uuid.empty() && !timestamp_str.empty() && !message_body.empty()) {
		time_t timestamp = std::stol(timestamp_str);
		client_private_message_print_messages(sender_uuid.c_str(), timestamp, message_body.c_str());
	}
}

}
