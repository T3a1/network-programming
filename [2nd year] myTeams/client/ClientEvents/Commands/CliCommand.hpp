/*
** EPITECH PROJECT, 2026
** G-NWP-400-LIL-4-1-myteams-2
** File description:
** CliCommand
*/

#pragma once
#include "Client.hpp"
#include <string>
#include <iostream>

namespace AEL {
    class CliCommand {
        public:
            virtual ~CliCommand() = default;
            virtual bool checkArgs(const std::string& args) = 0;
            virtual void handleCommand([[maybe_unused]]Client& client,
                [[maybe_unused]]const std::string& args) = 0;
            virtual void readInfo(const std::string&) {}
    };
}
