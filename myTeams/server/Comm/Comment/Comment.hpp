/*
** EPITECH PROJECT, 2026
** myteams
** File description:
** Comment
*/

#pragma once
#include "Comm.hpp"
#include <iostream>

namespace AEL
{
    class Comment : public Comm {
        public:
            Comment(const std::string name, const std::string description, const std::string threadUuid, const std::string authorUuid) : Comm(name, description), _threadUuid(threadUuid), _authorUuid(authorUuid) {};
            Comment(const std::string name, const std::string description, const std::string threadUuid, const std::string authorUuid, const std::string& uuid_str) : Comm(name, description, uuid_str), _threadUuid(threadUuid), _authorUuid(authorUuid) {};
            virtual ~Comment() = default;

            commtype_t getType() const override { return COMMENT; };
            const std::string& getThreadUUID() const { return _threadUuid; };
            const std::string& getAuthorUUID() const { return _authorUuid; };
    
        private:
            std::string _threadUuid;
            std::string _authorUuid;
    };
} // namespace AEL
