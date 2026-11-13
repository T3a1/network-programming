/*
** EPITECH PROJECT, 2026
** myteams
** File description:
** Thread
*/

#pragma once
#include "Comm.hpp"

namespace AEL
{
    class Thread : public Comm {
        public:
            Thread(const std::string name, const std::string description, const std::string& channel_uuid, const std::string& author_uuid)
                : Comm(name, description), _channel_uuid(channel_uuid), _author_uuid(author_uuid) {};
            Thread(const std::string name, const std::string description, const std::string& channel_uuid, const std::string& thread_uuid, const std::string& author_uuid)
                : Comm(name, description, thread_uuid), _channel_uuid(channel_uuid), _author_uuid(author_uuid) {};
            virtual ~Thread() = default;

            commtype_t getType() const override { return THREAD; };
            const std::string& getChannelUUID() const { return _channel_uuid; };
            const std::string& getAuthorUUID() const { return _author_uuid; };
    
        private:
            std::string _channel_uuid;
            std::string _author_uuid;
    };
} // namespace AEL
