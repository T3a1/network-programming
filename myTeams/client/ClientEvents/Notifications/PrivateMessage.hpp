/*
** EPITECH PROJECT, 2026
** G-NWP-400-LIL-4-1-myteams-2
** File description:
** PrivateMessage
*/

#pragma once
#include "Notification.hpp"

namespace AEL {
    class PrivateMessage : public Notification {
        public:
            void handle([[maybe_unused]]const std::string& response) override;
    };
}
