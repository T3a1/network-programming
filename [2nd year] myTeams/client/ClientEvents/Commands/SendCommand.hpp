/*
** EPITECH PROJECT, 2026
** G-NWP-400-LIL-4-1-myteams-2
** File description:
** SendCommand
*/

#pragma once
#include "CliCommand.hpp"
#include <iostream>

namespace AEL {
    class SendCommand : public CliCommand {
        public:
            bool checkArgs(const std::string& args) override;
            void handleCommand([[maybe_unused]]Client& client,
                [[maybe_unused]]const std::string& args) override;
            void readInfo(const std::string& response) override;

        private:
            std::string _uuid;
            std::string _message;
    };
}
