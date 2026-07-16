/*
** EPITECH PROJECT, 2026
** myteams
** File description:
** Server
*/

#include "Server.hpp"

bool AEL::Server::_server_loop;

AEL::Server::Server(uint16_t port)
    :_port(port),_poll_fd(PollFD(-1, 0)), _control_socket(port), _engine(Engine())
{
    _server_loop = true;
    std::cout << "Starting server on port " << _port << std::endl;
}

AEL::Server::~Server()
{
}

bool AEL::Server::endsWithCRLF(const std::vector<char>& vec, std::size_t read_size) {
    if (read_size == 0)
        return false;
    if (read_size == 1)
        return true;
    return vec[read_size - 2] == '\r' && vec[read_size - 1] == '\n';
}

void AEL::Server::signal_handler([[maybe_unused]] int signum)
{
    _server_loop = false;
}

bool AEL::Server::readData(int fd)
{
    std::vector<char> buffer(MAX_READ);
    int read_size;

    read_size = read(fd, buffer.data(), MAX_READ);
    if (read_size == -1)
        throw Minor("Could not read data");
    if (endsWithCRLF(buffer, read_size)) {
        if (read_size == 1)
            return true;
        buffer.data()[read_size - 2] = '\0';
    }
    else {
        if (read_size == 0)
            return true;
        buffer.data()[read_size - 1] = '\0';
    }
    std::vector<std::string> final_buffer;
    if (read_size <= 2) {
        final_buffer = {std::string(buffer.data())};
        _engine.exec(final_buffer, fd);
        return false;
    }
    std::stringstream iss(buffer.data());
    for (std::string param; std::getline(iss, param, ' ');)
        final_buffer.push_back(param);
    return _engine.exec(final_buffer, fd);
}

void AEL::Server::handleEvents()
{
    auto& fds = _poll_fd.getFds();

    for (auto fd : fds) {
        if (fd.revents & POLLIN) {
            if (fd.fd == _control_socket.getSocketFD()) {
                int new_fd = _control_socket.accept(fd.fd);
                _poll_fd.add(new_fd);
                _engine.add(new_fd);
            } else {
                if (readData(fd.fd)) {
                    _engine.remove(fd.fd);
                    _poll_fd.remove(fd);
                }
            }
        }
    }
}

void AEL::Server::run()
{
    _control_socket.bind(AF_INET, INADDR_ANY);
    _control_socket.listen();
    _poll_fd = PollFD(_control_socket.getSocketFD(), POLLIN);
    _engine.load();
    std::signal(SIGINT, &AEL::Server::signal_handler);
    while (_server_loop) {
        try {
            if (_poll_fd.fetchPoll() > 0)
                handleEvents();
        } catch (Minor& e) {
            std::cout << "Minor : " << e.what() << std::endl;
        } catch (Major& e) {
            std::cout << "Major: " << e.what() << std::endl;
            _server_loop = false;
        }
    }
    _engine.save();
}
