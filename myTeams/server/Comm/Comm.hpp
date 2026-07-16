/*
** EPITECH PROJECT, 2026
** myteams
** File description:
** Comm
*/

#pragma once
    #include <uuid/uuid.h>
    #include <memory>
    #include <vector>
    #include <iostream>

    #include "User.hpp"

    #define UUID_SIZE 37

namespace AEL
{
    typedef enum {
        TEAM,
        CHANNEL,
        THREAD,
        COMMENT
    } commtype_t;

    class Comm {
        public:
            Comm(const std::string name, const std::string description)
                : _name(name), _description(description) {
                uuid_generate_random(_uuid);
                std::time(&_timestamp);
            };
            Comm(const std::string name, const std::string description, const std::string& uuid_str)
                : _name(name), _description(description) {
                if (uuid_parse(uuid_str.c_str(), _uuid) != 0)
                    throw Critical("A comm from the .AEL file has an invalid uuid");
                std::time(&_timestamp);
            };
            ~Comm() = default;

            virtual commtype_t getType() const = 0;
            const uuid_t& getUUID() const { return _uuid; };
            std::string getUUID_str() const
            {
                char uuid_str[UUID_SIZE];
                uuid_unparse(_uuid, uuid_str);
                return std::string(uuid_str);
            };
            const std::string getName() const { return _name; };
            const std::string getDescription() const { return _description; };
            std::time_t getTimestamp() const { return _timestamp; };

        protected:
            uuid_t _uuid;
            const std::string _name;
            const std::string _description;
            std::time_t _timestamp;

    };
} // namespace AEL
