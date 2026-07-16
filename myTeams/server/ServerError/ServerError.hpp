/*
** EPITECH PROJECT, 2026
** myteams
** File description:
** ServerError
*/

#pragma once
#include <exception>
#include <iostream>

#undef EXIT_FAILURE
#define EXIT_FAILURE 84

// Length constraints for input validation
#define MAX_NAME_LENGTH 32
#define MAX_DESCRIPTION_LENGTH 255
#define MAX_BODY_LENGTH 512

// 2xx - Client login/logout events and basic errors
// 2xx - Client login/logout events and basic errors  3/3
#define M220(user_uuid, user_name) ("220 " + std::string(user_uuid) + " \"" + std::string(user_name) + "\"")
#define M221(user_uuid, user_name) ("221 " + std::string(user_uuid) + " \"" + std::string(user_name) + "\"")
#define M230 "230"

// 2x00 - Client private messages and notifications  5/5
#define M2000(user_uuid, message_body) ("2000 " + std::string(user_uuid) + " \"" + std::string(message_body) + "\"")
#define M2001(team_uuid, thread_uuid, user_uuid, reply_body) ("2001 " + std::string(team_uuid) + " " + std::string(thread_uuid) + " " + std::string(user_uuid) + " \"" + std::string(reply_body) + "\"")
#define M2002(team_uuid, team_name, team_description) ("2002 " + std::string(team_uuid) + " \"" + std::string(team_name) + "\" \"" + std::string(team_description) + "\"")
#define M2003(channel_uuid, channel_name, channel_description) ("2003 " + std::string(channel_uuid) + " \"" + std::string(channel_name) + "\" \"" + std::string(channel_description) + "\"")
#define M2004(thread_uuid, user_uuid, thread_timestamp, thread_title, thread_body) ("2004 " + std::string(thread_uuid) + " " + std::string(user_uuid) + " " + std::to_string(thread_timestamp) + " \"" + std::string(thread_title) + "\" \"" + std::string(thread_body) + "\"")

// 21xx - Client list printing (users, teams, channels, threads, replies, messages)  0/6
#define M2100(user_uuid, user_name, user_status) ("2100 " + std::string(user_uuid) + " \"" + std::string(user_name) + "\" " + std::string(user_status))
#define M2101(team_uuid, team_name, team_description) ("2101 " + std::string(team_uuid) + " \"" + std::string(team_name) + "\" \"" + std::string(team_description) + "\"")
#define M2102(channel_uuid, channel_name, channel_description) ("2102 " + std::string(channel_uuid) + " \"" + std::string(channel_name) + "\" \"" + std::string(channel_description) + "\"")
#define M2103(thread_uuid, user_uuid, thread_timestamp, thread_title, thread_body) ("2103 " + std::string(thread_uuid) + " " + std::string(user_uuid) + " " + std::to_string(thread_timestamp) + " \"" + std::string(thread_title) + "\" \"" + std::string(thread_body) + "\"")
#define M2104(thread_uuid, user_uuid, reply_timestamp, reply_body) ("2104 " + std::string(thread_uuid) + " " + std::string(user_uuid) + " " + std::to_string(reply_timestamp) + " \"" + std::string(reply_body) + "\"")
#define M2105(sender_uuid, message_timestamp, message_body) ("2105 " + std::string(sender_uuid) + " " + std::to_string(message_timestamp) + " \"" + std::string(message_body) + "\"")

// 22xx - Client single item printing (user, team, channel, thread info)  0/4
#define M2200(user_uuid, user_name, user_status) ("2200 " + std::string(user_uuid) + " \"" + std::string(user_name) + "\" " + std::string(user_status))
#define M2201(team_uuid, team_name, team_description) ("2201 " + std::string(team_uuid) + " \"" + std::string(team_name) + "\" \"" + std::string(team_description) + "\"")
#define M2202(channel_uuid, channel_name, channel_description) ("2202 " + std::string(channel_uuid) + " \"" + std::string(channel_name) + "\" \"" + std::string(channel_description) + "\"")
#define M2203(thread_uuid, user_uuid, thread_timestamp, thread_title, thread_body) ("2203 " + std::string(thread_uuid) + " " + std::string(user_uuid) + " " + std::to_string(thread_timestamp) + " \"" + std::string(thread_title) + "\" \"" + std::string(thread_body) + "\"")

// 23xx - Client creation success messages (team, channel, thread, reply)  4/4
#define M2300(team_uuid, team_name, team_description) ("2300 " + std::string(team_uuid) + " \"" + std::string(team_name) + "\" \"" + std::string(team_description) + "\"")
#define M2301(channel_uuid, channel_name, channel_description) ("2301 " + std::string(channel_uuid) + " \"" + std::string(channel_name) + "\" \"" + std::string(channel_description) + "\"")
#define M2302(thread_uuid, user_uuid, thread_timestamp, thread_title, thread_body) ("2302 " + std::string(thread_uuid) + " " + std::string(user_uuid) + " " + std::to_string(thread_timestamp) + " \"" + std::string(thread_title) + "\" \"" + std::string(thread_body) + "\"")
#define M2303(thread_uuid, user_uuid, reply_timestamp, reply_body) ("2303 " + std::string(thread_uuid) + " " + std::string(user_uuid) + " " + std::to_string(reply_timestamp) + " \"" + std::string(reply_body) + "\"")

// 24xx - Client subscription management  2/2
#define M2400(user_uuid, team_uuid) ("2400 " + std::string(user_uuid) + " " + std::string(team_uuid))
#define M2401(user_uuid, team_uuid) ("2401 " + std::string(user_uuid) + " " + std::string(team_uuid))

// 3xx - Server info/status codes 1/1
#define M320(description) ("320 \"" + std::string(description) + "\"")

// 4xx - Error codes
#define M400(msg) ("400: " + std::string(msg))
#define M410(msg) ("410: " + std::string(msg))
#define M411(msg) ("411: " + std::string(msg))
#define M412(msg) ("412: " + std::string(msg))
#define M413(msg) ("413: " + std::string(msg))
#define M500(msg) ("500: " + std::string(msg))
#define M530(msg) ("530: " + std::string(msg))

// 40xx - Client error codes
#define M4001(team_uuid) ("4001 " + std::string(team_uuid))
#define M4002(channel_uuid) ("4002 " + std::string(channel_uuid))
#define M4003(thread_uuid) ("4003 " + std::string(thread_uuid))
#define M4004(user_uuid) ("4004 " + std::string(user_uuid))
#define M4005 "4005"

namespace AEL {
    class AServerError : public std::exception {
        public:
            AServerError(std::string message) :_message(message) {};
            const char *what() const noexcept override {return _message.c_str();};
        private:
            std::string _message;
    };

    class Minor : public AServerError {
        public:
            Minor(std::string message) : AServerError(std::move(message)) {}
    };

    class Major : public AServerError {
        public:
            Major(std::string message) : AServerError(std::move(message)) {}
    };

    class Critical : public AServerError {
        public:
            Critical(std::string message) : AServerError(std::move(message)) {}
    };
}
