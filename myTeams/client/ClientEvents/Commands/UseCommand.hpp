/*
** EPITECH PROJECT, 2026
** G-NWP-400-LIL-4-1-myteams-2
** File description:
** UseCommand
*/

#pragma once
#include "CliCommand.hpp"
#include <iostream>
#include <vector>

namespace AEL {
    class UseCommand : public CliCommand {
        public:
            bool checkArgs(const std::string& args) override;
            void handleCommand([[maybe_unused]]Client& client,
                [[maybe_unused]]const std::string& args) override;
            void readInfo(const std::string& response) override;

        private:
            std::vector<std::string> _uuids;
    };
}
