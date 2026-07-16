/*
** EPITECH PROJECT, 2026
** G-NWP-400-LIL-4-1-myteams-2
** File description:
** ClientError
*/

#pragma once
#include <exception>
#include <iostream>

namespace AEL {
    class ClientError : public std::exception {
        public:
            ClientError(std::string message) :_message(message) {};
            const char *what() const noexcept override { return _message.c_str(); };

        private:
            std::string _message;
    };
}
