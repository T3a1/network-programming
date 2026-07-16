/*
** EPITECH PROJECT, 2026
** myChannels
** File description:
** Channel
*/

#pragma once
#include "Comm.hpp"

namespace AEL
{
    class Channel : public Comm {
        public:
            Channel(const std::string name, const std::string description, const std::string& team_uuid = "") 
                : Comm(name, description), _team_uuid(team_uuid) {};
            Channel(const std::string name, const std::string description, const std::string& team_uuid, const std::string& channel_uuid)
                : Comm(name, description, channel_uuid), _team_uuid(team_uuid) {};
            virtual ~Channel() = default;

            commtype_t getType() const override { return CHANNEL; };
            const std::string& getTeamUUID() const { return _team_uuid; };
    
        private:
            std::string _team_uuid;
    };
} // namespace AEL
