/*
** EPITECH PROJECT, 2026
** myteams
** File description:
** PollFD
*/

#pragma once
#include <vector>
#include <poll.h>
#include <algorithm>
#include <unistd.h>

#include "ServerError.hpp"

namespace AEL
{
    class PollFD {
        public:
            PollFD(int fd, short events);
            ~PollFD();

            void add(int new_fd);
            void remove(struct pollfd to_remove);
            int fetchPoll();
            std::vector<struct pollfd>& getFds() { return _fds; };

        private:
            int _fd;
            short _events;
            std::vector<struct pollfd> _fds;
    };
} // namespace AEL
