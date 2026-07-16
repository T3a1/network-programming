/*
** EPITECH PROJECT, 2026
** G-NWP-400-LIL-4-1-myteams-2
** File description:
** UnauthorizedCommand
*/

#pragma once
#include "Notification.hpp"

namespace AEL {
    class UnauthorizedCommand : public Notification {
        public:
            void handle([[maybe_unused]]const std::string& response) override;
    };
}
