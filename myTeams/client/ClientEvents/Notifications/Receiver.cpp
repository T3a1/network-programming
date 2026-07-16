/*
** EPITECH PROJECT, 2026
** G-NWP-400-LIL-4-1-myteams-2
** File description:
** Receiver
*/

#include "Receiver.hpp"
#include "Notification.hpp"
#include "PrivateMessage.hpp"
#include "ThreadReply.hpp"
#include "TeamCreated.hpp"
#include "ChannelCreated.hpp"
#include "ThreadCreated.hpp"
#include "UnknownTeam.hpp"
#include "UnknownChannel.hpp"
#include "UnknownThread.hpp"
#include "UnknownUser.hpp"
#include "UnauthorizedCommand.hpp"
#include "AlreadyExists.hpp"
#include <iostream>
#include <sstream>
#include <unordered_map>
#include <memory>

namespace AEL {

void Receiver::display(const std::string& message) {
    std::istringstream iss(message);
    std::string code;
    iss >> code;

    static std::unordered_map<std::string, std::unique_ptr<Notification>> handlers;
    if (handlers.empty()) {
        handlers["230"] = std::make_unique<AlreadyExists>();
        handlers["2000"] = std::make_unique<PrivateMessage>();
        handlers["2001"] = std::make_unique<ThreadReply>();
        handlers["2002"] = std::make_unique<TeamCreated>();
        handlers["2003"] = std::make_unique<ChannelCreated>();
        handlers["2004"] = std::make_unique<ThreadCreated>();
        handlers["4001"] = std::make_unique<UnknownTeam>();
        handlers["4002"] = std::make_unique<UnknownChannel>();
        handlers["4003"] = std::make_unique<UnknownThread>();
        handlers["4004"] = std::make_unique<UnknownUser>();
        handlers["4005"] = std::make_unique<UnauthorizedCommand>();
    }

    auto it = handlers.find(code);
    if (it != handlers.end())
        it->second->handle(message);
    else
        std::cout << message << std::endl;
    std::cout.flush();
}

}
