/*
** EPITECH PROJECT, 2026
** myteams
** File description:
** main
*/

#include <iostream>
#include "Server.hpp"

int main(int ac, char **av)
{
    if (ac != 2) {
        return EXIT_FAILURE;
    }
    try {
        AEL::Server server(atol(av[1]));
        server.run();
    } catch (AEL::Critical& e) {
        std::cout << "Critical : " << e.what() << std::endl;
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}
