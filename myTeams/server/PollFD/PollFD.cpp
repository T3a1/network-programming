/*
** EPITECH PROJECT, 2026
** myteams
** File description:
** PollFD
*/

#include "PollFD.hpp"

AEL::PollFD::PollFD(int fd, short events)
    :_fd(fd), _events(events)
{
    _fds = {{_fd, events, 0}};
}

AEL::PollFD::~PollFD()
{
}

void AEL::PollFD::add(int new_fd)
{
    _fds.push_back({new_fd, _events, 0});
}

void AEL::PollFD::remove(struct pollfd to_remove)
{
    auto it = std::find_if(_fds.begin(), _fds.end(), [&to_remove](const struct pollfd& fd) { return fd.fd == to_remove.fd; });
    if (it == _fds.end())
        throw Major("Could not remove user");
    close(it->fd);
    _fds.erase(it);
}

int AEL::PollFD::fetchPoll()
{
    return poll(_fds.data(), _fds.size(), -1);
}
