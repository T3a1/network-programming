/*
** EPITECH PROJECT, 2026
** G-NWP-400-LIL-4-1-myteams-2
** File description:
** Notification
*/

#pragma once
#include <string>
#include <iostream>
#include <sstream>
extern "C" {
	#include "logging_client.h"
}

namespace AEL {
    class Notification {
        public:
            virtual ~Notification() = default;
            virtual void handle([[maybe_unused]]const std::string& response) = 0;
    };
}
