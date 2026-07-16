/*
** EPITECH PROJECT, 2026
** myteams
** File description:
** Server
*/

#pragma once
    #include <iostream>
    #include <sys/socket.h>
    #include <sstream>
    #include <string>
    #include <netinet/in.h>
    #include <arpa/inet.h>
    #include <unistd.h>
    #include <poll.h>
    #include <vector>
    #include <csignal>

    #include "PollFD.hpp"
    #include "ServerError.hpp"
    #include "Socket.hpp"
    #include "Engine.hpp"

    #define QUEUE_SIZE 10000
    #define MAX_READ 4096

namespace AEL {
    class Server {
    public:
        Server(uint16_t port);
        ~Server();

        void run();

    private:
        uint16_t _port;
        PollFD _poll_fd;
        static bool _server_loop;
        Socket _control_socket;
        Engine _engine;

        void handleEvents();
        bool readData(int fd);
        static void signal_handler([[maybe_unused]] int signum);
        bool endsWithCRLF(const std::vector<char>& vec, std::size_t read_size);
    };
}
