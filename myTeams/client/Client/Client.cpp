/*
** EPITECH PROJECT, 2026
** G-NWP-400-LIL-4-1-myteams-2
** File description:
** Client
*/

#include "Client.hpp"
#include "Receiver.hpp"
#include "CliCommand.hpp"
#include "LoginCommand.hpp"
#include "LogoutCommand.hpp"
#include "HelpCommand.hpp"
#include "SendCommand.hpp"
#include "MessagesCommand.hpp"
#include "SubscribeCommand.hpp"
#include "SubscribedCommand.hpp"
#include "UnsubscribeCommand.hpp"
#include "UserCommand.hpp"
#include "UsersCommand.hpp"
#include "UseCommand.hpp"
#include "CreateCommand.hpp"
#include "ListCommand.hpp"
#include "InfoCommand.hpp"
#include "CleanCommand.hpp"
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <cstring>
#include <unistd.h>
#include <iostream>
#include <sstream>
#include <unordered_map>
#include <memory>
#include <functional>
#include <poll.h>

namespace AEL {

Client::Client(const std::string &ip, int port)
    : _ip(ip), _port(port), _socketFd(-1)
{
}

Client::~Client() {
    if (_socketFd != -1)
        close(_socketFd);
}

bool Client::connectToServer() {
    _socketFd = socket(AF_INET, SOCK_STREAM, 0);

    if (_socketFd < 0) {
        std::cout << "socket" << std::endl;
        return false;
    }

    struct sockaddr_in serv_address;
    std::memset(&serv_address, 0, sizeof(serv_address));
    serv_address.sin_family = AF_INET;
    serv_address.sin_port = htons(_port);

    if (inet_pton(AF_INET, _ip.c_str(), &serv_address.sin_addr) <= 0) {
        std::cout << "inet_pton" << std::endl;
        return false;
    }

    if (connect(_socketFd, (struct sockaddr *)&serv_address, sizeof(serv_address)) < 0) {
        std::cout << "connect" << std::endl;
        return false;
    }

    std::cout << "Connected to server." << std::endl;
    return true;
}

void Client::run() {
    std::string line;
    std::cout << "\nAEL $> ";
    fflush(stdout);
    while (!isLoggedOut()) {
        struct pollfd fds[2];
        fds[0].fd = _socketFd;
        fds[0].events = POLLIN;
        fds[1].fd = STDIN_FILENO;
        fds[1].events = POLLIN;

        int ret = poll(fds, 2, -1);
        if (ret < 0)
            continue;

        if (fds[0].revents & POLLIN) {
            readServerResponse();
            std::cout << "\nAEL $> ";
            fflush(stdout);
        }
        if (fds[1].revents & POLLIN) {
            if (!std::getline(std::cin, line))
                break;
            if (line.empty()) {
                std::cout << "\nAEL $> ";
                fflush(stdout);
                continue;
            }
            handleCommand(line);
            if (isLoggedOut())
                break;
            std::cout << "\nAEL $> ";
            fflush(stdout);
        }
    }
}

void Client::handleCommand(const std::string& command) {
    std::stringstream ss(command);
    std::string cmd;
    ss >> cmd;

    std::string args;
    std::getline(ss, args);
    if (!args.empty() && args.front() == ' ')
        args.erase(0, 1);

    static std::unordered_map<std::string, std::unique_ptr<CliCommand>> commands;
    if (commands.empty()) {
        commands["/login"] = std::make_unique<LoginCommand>();
        commands["/logout"] = std::make_unique<LogoutCommand>();
        commands["/help"] = std::make_unique<HelpCommand>();
        commands["/send"] = std::make_unique<SendCommand>();
        commands["/messages"] = std::make_unique<MessagesCommand>();
        commands["/subscribe"] = std::make_unique<SubscribeCommand>();
        commands["/subscribed"] = std::make_unique<SubscribedCommand>();
        commands["/unsubscribe"] = std::make_unique<UnsubscribeCommand>();
        commands["/user"] = std::make_unique<UserCommand>();
        commands["/users"] = std::make_unique<UsersCommand>();
        commands["/use"] = std::make_unique<UseCommand>();
        commands["/create"] = std::make_unique<CreateCommand>();
        commands["/list"] = std::make_unique<ListCommand>();
        commands["/info"] = std::make_unique<InfoCommand>();
        commands["/clean"] = std::make_unique<CleanCommand>();
    }

    auto it = commands.find(cmd);
    if (it != commands.end()) {
        if (it->second->checkArgs(args)) {
            it->second->handleCommand(*this, args);
            if (cmd != "/help")
                readServerResponse();
            if (cmd == "/logout")
                setLoggedOut(true);
        } else
            std::cerr << "Invalid arguments for " << cmd << std::endl;
    } else
        std::cerr << "Unknown command: " << cmd << std::endl;
}
void Client::readServerResponse() {
    char buffer[4096] = {0};
    ssize_t valread = recv(_socketFd, buffer, sizeof(buffer) - 1, 0);
    if (valread == 0) {
        std::cout << "Server closed the connection." << std::endl;
        _isLoggedOut = true;
        return;
    }
    else if (valread > 0) {
        buffer[valread] = '\0';
        std::string responseBuffer(buffer);

        static std::unordered_map<std::string, std::function<void(const std::string&)>> handlers = {
            {"220", [this](const std::string& response) { static LoginCommand loginCmd; loginCmd.readInfo(response); }},
            {"221", [this](const std::string& response) { static LogoutCommand logoutCmd; logoutCmd.readInfo(response); }},
            {"230", [this](const std::string& response) {Receiver::display(response); }},
            {"999", [this](const std::string& response) { static UseCommand useCmd; useCmd.readInfo(response); }},
            {"2300", [this](const std::string& response) { static CreateCommand createCmd; createCmd.readInfo(response); }},
            {"2301", [this](const std::string& response) { static CreateCommand createCmd; createCmd.readInfo(response); }},
            {"2302", [this](const std::string& response) { static CreateCommand createCmd; createCmd.readInfo(response); }},
            {"2303", [this](const std::string& response) { static CreateCommand createCmd; createCmd.readInfo(response); }},
            {"2400", [this](const std::string& response) { static SubscribeCommand subCmd; subCmd.readInfo(response); }},
            {"2401", [this](const std::string& response) { static UnsubscribeCommand unsubCmd; unsubCmd.readInfo(response); }},
            {"1004", [this](const std::string& response) { static SendCommand sendCmd; sendCmd.readInfo(response); }},
            {"2000", [this](const std::string& response) { Receiver::display(response); }},
            {"2001", [this](const std::string& response) { Receiver::display(response); }},
            {"2002", [this](const std::string& response) { Receiver::display(response); }},
            {"2003", [this](const std::string& response) { Receiver::display(response); }},
            {"2004", [this](const std::string& response) { Receiver::display(response); }},
            {"2100", [this](const std::string& response) { static UsersCommand usersCmd; usersCmd.readInfo(response); }},
            {"2101", [this](const std::string& response) { static SubscribedCommand subCmd; subCmd.readInfo(response); }},
            {"2102", [this](const std::string& response) { static ListCommand listCmd; listCmd.readInfo(response); }},
            {"2103", [this](const std::string& response) { static ListCommand listCmd; listCmd.readInfo(response); }},
            {"2104", [this](const std::string& response) { static ListCommand listCmd; listCmd.readInfo(response); }},
            {"2105", [this](const std::string& response) { static MessagesCommand msgCmd; msgCmd.readInfo(response); }},
            {"2200", [this](const std::string& response) { static UserCommand userCmd; userCmd.readInfo(response); }},
            {"2201", [this](const std::string& response) { static InfoCommand infoCmd; infoCmd.readInfo(response); }},
            {"2202", [this](const std::string& response) { static InfoCommand infoCmd; infoCmd.readInfo(response); }},
            {"2203", [this](const std::string& response) { static InfoCommand infoCmd; infoCmd.readInfo(response); }},
            {"400", [this](const std::string& response) { std::cout << "Error: " << response << std::endl; }},
            {"410", [this](const std::string& response) { std::cout << "Error: " << response << std::endl; }},
            {"411", [this](const std::string& response) { std::cout << "Error: " << response << std::endl; }},
            {"412", [this](const std::string& response) { std::cout << "Error: " << response << std::endl; }},
            {"413", [this](const std::string& response) { std::cout << "Error: " << response << std::endl; }},
            {"500", [this](const std::string& response) { std::cout << "Error: " << response << std::endl; }},
            {"530", [this](const std::string& response) { std::cout << "Error: " << response << std::endl; }},
            {"320", [this](const std::string& response) { std::cout << "Info: " << response << std::endl; }},
            {"4001", [this](const std::string& response) {Receiver::display(response); }},
            {"4002", [this](const std::string& response) {Receiver::display(response); }},
            {"4003", [this](const std::string& response) {Receiver::display(response); }},
            {"4004", [this](const std::string& response) {Receiver::display(response); }},
            {"4005", [this](const std::string& response) {Receiver::display(response); }},
        };

        std::istringstream iss(responseBuffer);
        std::string line;
        while (std::getline(iss, line)) {
            if (line.empty())
                continue;

            std::istringstream lineStream(line);
            std::string code;
            lineStream >> code;
            
            auto it = handlers.find(code);
            if (it != handlers.end())
                it->second(line);
            else
                std::cout << line << std::endl;
        }
    }
}

void Client::sendCommand(const std::string& command) {
    send(_socketFd, command.c_str(), command.length() + 1, 0);
}

void Client::setContext(const std::vector<std::string>& context) {
    _context = context;
}

const std::vector<std::string>& Client::getContext() const {
    return _context;
}

int Client::getContextLevel() const {
    return _context.size();
}

}
