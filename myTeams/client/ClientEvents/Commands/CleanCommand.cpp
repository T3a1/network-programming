/*
** EPITECH PROJECT, 2026
** G-NWP-400-LIL-4-1-myteams-2
** File description:
** CleanCommand
*/

#include "CleanCommand.hpp"
#include <cstdlib>
#include <iostream>

namespace AEL {

bool CleanCommand::checkArgs(const std::string& args) {
    if (!args.empty()) {
        std::cout << "Command /clean doesn't take any argument." << std::endl;
        return false;
    }
    return true;
}

void CleanCommand::handleCommand([[maybe_unused]]Client& client, [[maybe_unused]]const std::string& args) {
    std::cout << "\033[2J\033[H";
    std::cout << "AEL $> ";
    std::cout.flush();
}

}
