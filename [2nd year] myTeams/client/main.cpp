/*
** EPITECH PROJECT, 2026
** myteams
** File description:
** main
*/

#include "Client/Client.hpp"
#include "ClientError/ClientError.hpp"
#include <iostream>

void printUsage(const char *prog_name) {
    std::cout << "USAGE: " << prog_name << " ip port" << std::endl;
    std::cout << "\n ip is the server ip address on which the server socket listens" << std::endl;
    std::cout << " port is the port number on which the server socket listens" << std::endl;
}

int main(int argc, char **argv)
{
    if (argc == 2 && argv[1] == std::string("--help")) {
        printUsage(argv[0]);
        return EXIT_SUCCESS;
    }
    if (argc != 3) {
        std::cout << "Use \"./myteams_cli --help\" for further informations" << std::endl;
        return EXIT_FAILURE;
    }
    const char *ip = argv[1];
    int port = std::atoi(argv[2]);

    if (port <= 0) {
        std::cout << "Invalid port number." << std::endl;
        return EXIT_FAILURE;
    }

    try {
        AEL::Client client(ip, port);
        if (!client.connectToServer())
            return 84;
        client.run();
    } catch (AEL::ClientError& e) {
        std::cout << "Error : " << e.what() << std::endl;
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}
