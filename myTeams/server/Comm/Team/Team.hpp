/*
** EPITECH PROJECT, 2026
** myteams
** File description:
** Team
*/

#pragma once
#include "Comm.hpp"
#include <algorithm>

namespace AEL
{
    class Team : public Comm {
        public:
            Team(const std::string name, const std::string description) : Comm(name, description) {};
            Team(const std::string name, const std::string description, const std::string& uuid_str) : Comm(name, description, uuid_str) {};
            virtual ~Team() = default;

            commtype_t getType() const override { return TEAM; };
    };
} // namespace AEL