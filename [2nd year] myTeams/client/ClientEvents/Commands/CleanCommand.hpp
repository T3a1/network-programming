/*
** EPITECH PROJECT, 2026
** G-NWP-400-LIL-4-1-myteams-2
** File description:
** CleanCommand
*/

#pragma once
#include "CliCommand.hpp"

namespace AEL {
    class CleanCommand : public CliCommand {
        public:
            bool checkArgs(const std::string& args) override;
            void handleCommand([[maybe_unused]]Client& client,
                [[maybe_unused]]const std::string& args) override;
    };
}
