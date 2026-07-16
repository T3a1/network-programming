/*
** EPITECH PROJECT, 2026
** myteams
** File description:
** Engine
*/

#include "Engine.hpp"

AEL::Engine::Engine()
	:_teams(), _channels(), _threads(), _comments()
{
	std::filesystem::path user_file(USERFILE_PATH);

	if (!std::filesystem::exists(user_file))
		throw Critical("User file does not exist");
}

AEL::Engine::~Engine()
{
}

void AEL::Engine::sendToClient(User* user, std::string message)
{
	message += '\n';
	send(user->getFD(), message.data(), message.size(), MSG_CONFIRM);
}



bool AEL::Engine::checkContext([[maybe_unused]] std::vector<std::string> command, [[maybe_unused]] User* user)
{
	if (command.size() < 1) {
		sendToClient(user, M410("Invalid USE command"));
		throw Minor("Invalid USE command");
	}

	if (command.size() == 1) {
		user->setCurrentTeam("");
		user->setCurrentChannel("");
		user->setCurrentThread("");
		std::string response = "999   ";
		sendToClient(user, response);
		return false;
	}

	switch(command.size()) {
	case 2:
		USE(command[1], "", "", user);
		break;
	case 3:
		USE(command[1], command[2], "", user);
		break;
	case 4:
		USE(command[1], command[2], command[3], user);
		break;
	default:
		sendToClient(user, M410("Invalid USE command size"));
		throw Minor("Invalid USE command");
	}
	std::string response = "999 " + user->getCurrentTeam() + " " + user->getCurrentChannel() + " " + user->getCurrentThread();
	sendToClient(user, response);
	return false;
}



bool AEL::Engine::exec(std::vector<std::string> command, int current_fd)
{
	User* user = getUserByFD(current_fd);

	if (!user) {
		throw Minor("User not found for fd: " + std::to_string(current_fd));
	}
	command = splitCommand(command);

    static const std::unordered_map<std::string, std::function<bool(std::vector<std::string>, User*)>> creators = {
        {"CRT", [this](std::vector<std::string> command, User* user) { return CRT(command, user); }},
        {"CRC", [this](std::vector<std::string> command, User* user) { return CRC(command, user); }},
        {"CRH", [this](std::vector<std::string> command, User* user) { return CRH(command, user); }},
        {"CRR", [this](std::vector<std::string> command, User* user) { return CRR(command, user); }},
        {"SND", [this](std::vector<std::string> command, User* user) { return SND(command, user); }},
        {"SUB", [this](std::vector<std::string> command, User* user) { return SUB(command, user); }},
        {"UNS", [this](std::vector<std::string> command, User* user) { return UNS(command, user); }},
        {"USE", [this](std::vector<std::string> command, User* user) { return checkContext(command, user); }},
        {"LGI", [this](std::vector<std::string> command, User* user) { return LGI(command, user); }},
        {"LGO", [this](std::vector<std::string> command, User* user) { return LGO(command, user); }},
        {"USS", [this](std::vector<std::string> command, User* user) { return USS(command, user); }},
        {"USR", [this](std::vector<std::string> command, User* user) { return USR(command, user); }},
        {"SBD", [this](std::vector<std::string> command, User* user) { return SBD(command, user); }},
        {"MSG", [this](std::vector<std::string> command, User* user) { return MSG(command, user); }},
        {"LST", [this](std::vector<std::string> command, User* user) { return LST(command, user); }},
        {"IFU", [this](std::vector<std::string> command, User* user) { return INF(command, user); }},
        {"IFT", [this](std::vector<std::string> command, User* user) { return INF(command, user); }},
        {"IFC", [this](std::vector<std::string> command, User* user) { return INF(command, user); }},
        {"IFH", [this](std::vector<std::string> command, User* user) { return INF(command, user); }},
    };

    for (auto func : creators) {
    	if (func.first == command[0])
    		return func.second(command, user);
    }
    sendToClient(user, M500("Unknown command"));
    return false;
}


std::vector<std::string> AEL::Engine::splitCommand(std::vector<std::string> command)
{
	std::vector<std::string> new_command;
	std::string current_param;
	std::string raw_command;
	bool in_quotes = false;

	for (size_t i = 0; i < command.size(); ++i) {
		if (!raw_command.empty())
			raw_command += ' ';
		raw_command += command[i];
	}

	for (char c : raw_command) {
		if (c == '"') {
			in_quotes = !in_quotes;
			continue;
		}
		if (c == ' ' && !in_quotes) {
			if (!current_param.empty()) {
				new_command.push_back(current_param);
				current_param.clear();
			}
			continue;
		}
		current_param += c;
	}

	if (!current_param.empty())
		new_command.push_back(current_param);

	return new_command;
}
