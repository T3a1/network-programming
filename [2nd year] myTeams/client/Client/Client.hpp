/*
** EPITECH PROJECT, 2026
** G-NWP-400-LIL-4-1-myteams-2
** File description:
** Client
*/

#pragma once
#include <string>
#include <vector>
extern "C" {
	#include "logging_server.h"
	#include "logging_client.h"
}

namespace AEL {
    class Client {
        public:
            Client(const std::string &ip, int port);
            ~Client();
            bool connectToServer();
            void run();
            void handleCommand(const std::string& command);
            void sendCommand(const std::string& command);
            void setContext(const std::vector<std::string>& context);
            const std::vector<std::string>& getContext() const;
            int getContextLevel() const;
            void setLoggedOut(bool value) { _isLoggedOut = value; }
            bool isLoggedOut() const { return _isLoggedOut; }

        private:
            std::string _ip;
            int _port;
            int _socketFd;
            std::vector<std::string> _context;
            bool _isLoggedOut = false;
            void readServerResponse();
    };
}
