/*
** EPITECH PROJECT, 2026
** myteams
** File description:
** Socket
*/

#include "Socket.hpp"

AEL::Socket::Socket(uint16_t port)
    : _socket_fd(-1),_port(port), _is_control_socket(false)
{
    _socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (_socket_fd == -1) {
        throw Critical("Could not open socket");
    }
}

AEL::Socket::~Socket()
{
    if (_socket_fd != -1)
        close(_socket_fd);
}

void AEL::Socket::listen()
{
    if (::listen(_socket_fd, QUEUE_SIZE) == -1)
        throw Critical("Could not set socket to listening mode");
    _is_control_socket = true;
}

void AEL::Socket::bind(sa_family_t family, in_addr_t address)
{
    struct sockaddr_in bind_s;
    socklen_t addr_size_s;

    bind_s.sin_family = family;
    bind_s.sin_port = htons(_port);
    bind_s.sin_addr.s_addr = address;
    addr_size_s = sizeof(struct sockaddr_in);
    if (::bind(_socket_fd, (const struct sockaddr*) &bind_s, addr_size_s) == -1)
        throw Critical("Could not bind socket");
}

int AEL::Socket::accept(int fd_to_accept)
{
    struct sockaddr_in client_addr;
    socklen_t client_addr_len = sizeof(client_addr);

    int new_fd = ::accept(fd_to_accept, (struct sockaddr*) &client_addr, &client_addr_len);
    if (new_fd == -1)
        throw Minor("Entering connection seems to have diseappeared");
    std::cout << "New connection from " << inet_ntoa(client_addr.sin_addr) << ":" << ntohs(client_addr.sin_port) << std::endl;
    return new_fd;
}
