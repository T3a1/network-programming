/*
** EPITECH PROJECT, 2026
** G-NWP-400-LIL-4-1-myteams-2
** File description:
** HelpCommand
*/

#include "HelpCommand.hpp"

namespace AEL {

bool HelpCommand::checkArgs(const std::string& args) {
    if (!args.empty()) {
        std::cout << "Command /help doesn't take any argument." << std::endl;
        return false;
    }
    return true;
}

void HelpCommand::handleCommand([[maybe_unused]]Client& client, [[maybe_unused]]const std::string& args) {
    std::cout << "Available commands:" << std::endl;
    std::cout << "  /help\t\t\t\t\tshow help" << std::endl
              << "  /login [\"user_name\"]\t\t\tset the user_name used by client" << std::endl
              << "  /logout\t\t\t\tdisconnect the client from the server" << std::endl
              << "  /users\t\t\t\tget the list of all users that exist on the domain" << std::endl
              << "  /user [\"user_uuid\"]\t\t\tget details about the requested user" << std::endl
              << "  /send [\"user_uuid\"] [\"message_body\"]\tsend a message to specific user" << std::endl
              << "  /messages [\"user_uuid\"]\t\tlist all messages exchanged with the specified user" << std::endl
              << "  /subscribe [\"team_uuid\"]\t\tsubscribe to the events of a team and its sub directories "
              "(enable reception of all events from a team)" << std::endl
              << "  /subscribed ?[\"team_uuid\"]\t\tlist all subscribed teams or list all users subscribed to a team" << std::endl
              << "  /unsubscribe [\"team_uuid\"]\t\tunsubscribe from a team" << std::endl
              << "  /clean\t\t\t\t\tclear the terminal and redisplay the prompt" << std::endl;
}

}
