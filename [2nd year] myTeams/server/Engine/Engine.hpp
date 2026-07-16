/*
** EPITECH PROJECT, 2026
** myteams
** File description:
** Engine
*/

#pragma once
	#include <iostream>
	#include <functional>
	#include <vector>
	#include <memory>
	#include <fstream>
	#include <exception>
	#include <filesystem>
	#include <sys/socket.h>
	#include <algorithm>

	#include "ServerError.hpp"
	#include "Comm.hpp"
	#include "Team.hpp"
	#include "Channel.hpp"
	#include "Thread.hpp"
	#include "Comment.hpp"
	#include "User.hpp"
	#include "Parsing/Parsing.hpp"
	extern "C" {
		#include "logging_server.h"
	}

	#define USERFILE_PATH "./users.AEL"
	#define TEAMFILE_PATH "./teams.AEL"
	#define CHANNELFILE_PATH "./channels.AEL"
	#define THREADFILE_PATH "./threads.AEL"
	#define COMMENTFILE_PATH "./comments.AEL"
	#define PRIVATE_MESSAGEFILE_PATH "./messages.AEL"

namespace AEL
{
	struct Message {
		std::string sender_uuid;
		std::string receiver_uuid;
		std::string body;
		time_t timestamp;
	};

	class Engine {
		public:
			Engine();
			~Engine();
			bool exec(std::vector<std::string> command, int current_fd);
			void save();
			void load();
			void add(int fd);
			void remove(int fd);
			void sendToClient(User* user, std::string message);
			User* getUserByFD(int fd);

		private:
			bool CRT([[maybe_unused]] std::vector<std::string> command, [[maybe_unused]] User* user);
			bool CRC([[maybe_unused]] std::vector<std::string> command, [[maybe_unused]] User* user);
			bool CRH([[maybe_unused]] std::vector<std::string> command, [[maybe_unused]] User* user);
			bool CRR([[maybe_unused]] std::vector<std::string> command, [[maybe_unused]] User* user);
			bool SND([[maybe_unused]] std::vector<std::string> command, [[maybe_unused]] User* user);
			bool SUB([[maybe_unused]] std::vector<std::string> command, [[maybe_unused]] User* user);
			bool UNS([[maybe_unused]] std::vector<std::string> command, [[maybe_unused]] User* user);
			bool checkContext([[maybe_unused]] std::vector<std::string> command, [[maybe_unused]] User* user);
			bool LGI([[maybe_unused]] std::vector<std::string> command, [[maybe_unused]] User* user);
			bool LGO([[maybe_unused]] std::vector<std::string> command, [[maybe_unused]] User* user);
			void USE(const std::string& team, const std::string& channel, const std::string& thread, User* user);
			void notifyTeamUsers(const std::string& team_uuid, const std::string& message);
			bool USS([[maybe_unused]] std::vector<std::string> command, [[maybe_unused]] User* user);
			bool USR([[maybe_unused]] std::vector<std::string> command, [[maybe_unused]] User* user);
			bool SBD([[maybe_unused]] std::vector<std::string> command, [[maybe_unused]] User* user);
			bool MSG([[maybe_unused]] std::vector<std::string> command, [[maybe_unused]] User* user);
			bool LST([[maybe_unused]] std::vector<std::string> command, [[maybe_unused]] User* user);
			bool INF([[maybe_unused]] std::vector<std::string> command, [[maybe_unused]] User* user);
		std::vector<std::string> splitCommand(std::vector<std::string> command);
		User* getUserByUsername(const std::string& username);
		User* getUserByUUID(const std::string& uuid);
		std::shared_ptr<Team> getTeamByUUID(const std::string& uuid);
		std::shared_ptr<Channel> getChannelByUUID(const std::string& uuid);
		std::shared_ptr<Thread> getThreadByUUID(const std::string& uuid);
		std::shared_ptr<Comment> getCommentByUUID(const std::string& uuid);
		void moveToOfflineIfLastSession(User* session_user);
		void logoutCurrentSessionAndMoveToOffline(
			std::vector<std::unique_ptr<User>>::iterator session_it);
		void removeAnonymousByFD(int fd);
		void notifyAuthenticatedUsers(const std::string& exclude_uuid, const std::string& response);
		void notifyAuthenticatedSessions(const std::string& response);
		void notifyAuthenticatedSessionsExceptFD(int excluded_fd, const std::string& response);

		std::vector<std::shared_ptr<Team>> _teams;
		std::vector<std::shared_ptr<Channel>> _channels;
		std::vector<std::shared_ptr<Thread>> _threads;
		std::vector<std::shared_ptr<Comment>> _comments;
		std::vector<Message> _private_messages;

			std::vector<std::unique_ptr<User>> _anonymous_users;
			std::vector<std::unique_ptr<User>> _offline_users;
			std::vector<std::unique_ptr<User>> _authenticated_users;
	};
}
