/*
** EPITECH PROJECT, 2026
** myteams
** File description:
** Socket
*/

#pragma once
#include "ServerError.hpp"
#include "PollFD.hpp"

#include <arpa/inet.h>
#include <sys/socket.h>

#define QUEUE_SIZE 10000

namespace AEL
{
    class Socket {
        public:
            Socket(uint16_t port);
            ~Socket();

            int getSocketFD() const { return _socket_fd; };
            int isControlSocket() const { return _is_control_socket; };
            int accept(int fd_to_accept);
            void bind(sa_family_t family = AF_INET, in_addr_t address = INADDR_ANY);
            void listen();
            void send(int fd, std::string message);

        private:
            int _socket_fd;
            uint16_t _port;
            bool _is_control_socket;
    };
} // namespace AEL
