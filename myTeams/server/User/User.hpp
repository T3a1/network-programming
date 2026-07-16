/*
** EPITECH PROJECT, 2026
** myteams
** File description:
** User
*/

#pragma once
    #include <uuid/uuid.h>
    #include <vector>
    #include <iostream>
    #include <algorithm>

    #include "ServerError.hpp"

    #define UUID_SIZE 37

namespace AEL {
    class User {
        public:

            User(std::string name, std::string uuid_str, int fd = -1) : _name(name), _fd(fd)
            {
                if (uuid_parse(uuid_str.c_str(), _uuid) != 0)
                    throw Critical("A user from the .AEL file has an invalid uuid");
                
            };
            ~User() = default;

            const std::string& getUsername() const { return _name; };
            const uuid_t& getUUID() const { return _uuid; };
            int getFD() const { return _fd; };
            void setName(std::string name) { _name = name; };
            void setFD(int fd) { _fd = fd; };
            void setUUID(uuid_t uuid) { uuid_copy(_uuid, uuid); };
            std::string getUUID_str() const
            {
                char uuid_str[UUID_SIZE];
                uuid_unparse(_uuid, uuid_str);
                return std::string(uuid_str);
            };

            void setCurrentTeam(const std::string& team_uuid) { _current_team_uuid = team_uuid; };
            void setCurrentChannel(const std::string& channel_uuid) { _current_channel_uuid = channel_uuid; };
            void setCurrentThread(const std::string& thread_uuid) { _current_thread_uuid = thread_uuid; };

            const std::string& getCurrentTeam() const { return _current_team_uuid; };
            const std::string& getCurrentChannel() const { return _current_channel_uuid; };
            const std::string& getCurrentThread() const { return _current_thread_uuid; };

            bool hasTeamContext() const { return !_current_team_uuid.empty(); };
            bool hasChannelContext() const { return !_current_channel_uuid.empty(); };
            bool hasThreadContext() const { return !_current_thread_uuid.empty(); };

            const std::vector<std::string>& getTeamUuids() const { return _team_uuids; };
            void addTeamUuid(const std::string& team_uuid) { _team_uuids.push_back(team_uuid); };
            void removeTeamUuid(const std::string& team_uuid) {
                auto it = std::find(_team_uuids.begin(), _team_uuids.end(), team_uuid);
                if (it != _team_uuids.end()) {
                    _team_uuids.erase(it);
                }
            };
            bool isInTeam(const std::string& team_uuid) const {
                return std::find(_team_uuids.begin(), _team_uuids.end(), team_uuid) != _team_uuids.end();
            };

        private:
            std::string _name;
            int _fd;
            uuid_t _uuid;
            std::vector<std::string> _team_uuids;
            std::string _current_team_uuid;
            std::string _current_channel_uuid;
            std::string _current_thread_uuid;
    };
}
