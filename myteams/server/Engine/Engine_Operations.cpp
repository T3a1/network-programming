/*
** EPITECH PROJECT, 2026
** myteams
** File description:
** Engine_Operations
*/

#include "Engine.hpp"

void AEL::Engine::load()
{
	Parsing userParser(USERFILE_PATH, TEAMFILE_PATH, CHANNELFILE_PATH, THREADFILE_PATH, COMMENTFILE_PATH, PRIVATE_MESSAGEFILE_PATH);
	std::vector<Parsing::User> users;
	std::vector<Parsing::Team> teams;
	std::vector<Parsing::privateMessages> privateMessages;

	try {
		users = userParser.getUsers();
		teams = userParser.getTeams();
		privateMessages = userParser.getPrivateMessages();


	} catch (std::exception& e) {
		throw Critical(e.what());
	}

	_anonymous_users.clear();
	_offline_users.clear();
	_authenticated_users.clear();
	_teams.clear();
	_channels.clear();
	_threads.clear();
	_comments.clear();
	_private_messages.clear();

	for (const auto& team : teams) {
		auto team_obj = std::make_shared<Team>(team.name, team.description, team.teamUuid);
		_teams.push_back(team_obj);

		for (const auto& channel : team.channels) {
			auto channel_obj = std::make_shared<Channel>(channel.name, channel.description, channel.teamUuid, channel.channelUuid);
			_channels.push_back(channel_obj);

			for (const auto& thread : channel.threads) {
				auto thread_obj = std::make_shared<Thread>(thread.name, thread.description, thread.channelUuid, thread.threadUuid, thread.authorUuid);
				_threads.push_back(thread_obj);

				for (const auto& comment : thread.comments) {
					auto comment_obj = std::make_shared<Comment>("", comment.body, comment.threadUuid, comment.authorUuid, comment.commentUuid);
					_comments.push_back(comment_obj);
				}
			}
		}
	}
	for (const auto& pm : privateMessages) {
		Message message{pm.senderUuid, pm.receiverUuid, pm.body, pm.timestamp};
		_private_messages.push_back(message);
		printf("Loaded private message from %s to %s: %s (%ld)\n", pm.senderUuid.c_str(), pm.receiverUuid.c_str(), pm.body.c_str(), pm.timestamp);
	}
	
	for (const auto& user : users) {
		std::string name = user.name;
		std::string uuid = user.userUuid;
		
		_offline_users.push_back(std::make_unique<User>(name, uuid, -1));

		for (const auto& team : teams)
			if (std::find_if(team.authorizedUsers.begin(), team.authorizedUsers.end(),
				[&](const auto& authorizedUser) { return authorizedUser.userUuid == uuid; }) != team.authorizedUsers.end())
				_offline_users.back()->addTeamUuid(team.teamUuid);

		server_event_user_loaded(uuid.c_str(), name.c_str());
	}
}

void AEL::Engine::save()
{
	// save users
	std::ofstream user_file(USERFILE_PATH);
	if (!user_file.is_open()) {
		std::cerr << "Could not open user file for saving" << std::endl;
	} else {
		std::vector<User*> all_users;
		for (const auto& user : _authenticated_users)
			all_users.push_back(user.get());
		for (const auto& user : _offline_users)
			all_users.push_back(user.get());

		for (size_t i = 0; i < all_users.size(); ++i) {
			const auto& user = all_users[i];
			user_file << "{\n";
			user_file << "\t\"name\": \"" << user->getUsername() << "\",\n";
			user_file << "\t\"userUuid\": \"" << user->getUUID_str() << "\"\n";
			user_file << "}";
			user_file << "\n";
		}
		user_file.close();
		std::cout << "Saved " << all_users.size() << " users to " << USERFILE_PATH << std::endl;
	}

	// save teams
	std::ofstream team_file(TEAMFILE_PATH);
	if (!team_file.is_open()) {
		std::cerr << "Could not open team file for saving" << std::endl;
	} else {
		for (const auto& team : _teams) {
			team_file << "{\n";
			team_file << "\t\"name\": \"" << team->getName() << "\",\n";
			team_file << "\t\"teamUuid\": \"" << team->getUUID_str() << "\",\n";
			team_file << "\t\"description\": \"" << team->getDescription() << "\",\n";
			team_file << "\t\"usersUuid\": [";
			
			std::vector<std::string> subscribed_users;
			std::vector<User*> all_users;
			for (const auto& user : _authenticated_users)
				all_users.push_back(user.get());
			for (const auto& user : _offline_users)
				all_users.push_back(user.get());
			for (const auto& user : all_users) {
				if (user->isInTeam(team->getUUID_str())) {
					subscribed_users.push_back(user->getUUID_str());
				}
			}
			
			for (size_t i = 0; i < subscribed_users.size(); ++i) {
				team_file << "\"" << subscribed_users[i] << "\"";
				if (i < subscribed_users.size() - 1) {
					team_file << ", ";
				}
			}
			team_file << "]\n";
			team_file << "}\n";
		}
		team_file.close();
		std::cout << "Saved " << _teams.size() << " teams to " << TEAMFILE_PATH << std::endl;
	}

	// save channels
	std::ofstream channel_file(CHANNELFILE_PATH);
	if (!channel_file.is_open()) {
		std::cerr << "Could not open channel file for saving" << std::endl;
	} else {
		for (const auto& team : _teams) {
			for (const auto& channel : _channels) {
				if (channel->getTeamUUID() != team->getUUID_str())
					continue;
				channel_file << "{\n";
				channel_file << "\t\"name\": \"" << channel->getName() << "\",\n";
				channel_file << "\t\"channelUuid\": \"" << channel->getUUID_str() << "\",\n";
				channel_file << "\t\"description\": \"" << channel->getDescription() << "\",\n";
				channel_file << "\t\"teamUuid\": \"" << channel->getTeamUUID() << "\"\n";
				channel_file << "}\n";
			}
		}
		channel_file.close();
		std::cout << "Saved " << _channels.size() << " channels to " << CHANNELFILE_PATH << std::endl;
	}

	// save threads
	std::ofstream thread_file(THREADFILE_PATH);
	if (!thread_file.is_open()) {
		std::cerr << "Could not open thread file for saving" << std::endl;
	} else {
		for (const auto& channel : _channels) {
			for (const auto& thread : _threads) {
				if (thread->getChannelUUID() != channel->getUUID_str())
					continue;
				thread_file << "{\n";
				thread_file << "\t\"name\": \"" << thread->getName() << "\",\n";
				thread_file << "\t\"threadUuid\": \"" << thread->getUUID_str() << "\",\n";
				thread_file << "\t\"description\": \"" << thread->getDescription() << "\",\n";
				thread_file << "\t\"channelUuid\": \"" << thread->getChannelUUID() << "\",\n";
				thread_file << "\t\"authorUuid\": \"" << thread->getAuthorUUID() << "\"\n";
				thread_file << "}\n";
			}
		}
		thread_file.close();
		std::cout << "Saved " << _threads.size() << " threads to " << THREADFILE_PATH << std::endl;
	}

	// save comments
	std::ofstream comment_file(COMMENTFILE_PATH);
	if (!comment_file.is_open()) {
		std::cerr << "Could not open comment file for saving" << std::endl;
	} else {
		for (const auto& thread : _threads) {
			for (const auto& comment : _comments) {
				if (comment->getThreadUUID() != thread->getUUID_str())
					continue;
				comment_file << "{\n";
				comment_file << "\t\"body\": \"" << comment->getDescription() << "\",\n";
				comment_file << "\t\"commentUuid\": \"" << comment->getUUID_str() << "\",\n";
				comment_file << "\t\"threadUuid\": \"" << comment->getThreadUUID() << "\",\n";
				comment_file << "\t\"authorUuid\": \"" << comment->getAuthorUUID() << "\"\n";
				comment_file << "}\n";
			}
		}
		comment_file.close();
		std::cout << "Saved " << _comments.size() << " comments to " << COMMENTFILE_PATH << std::endl;
	}

	// save private messages
	std::ofstream private_message_file(PRIVATE_MESSAGEFILE_PATH);
	if (!private_message_file.is_open()) {
		std::cerr << "Could not open private message file for saving" << std::endl;
	} else {
		for (const auto& msg : _private_messages) {
			private_message_file << "{\n";
			private_message_file << "\t\"senderUuid\": \"" << msg.sender_uuid << "\",\n";
			private_message_file << "\t\"receiverUuid\": \"" << msg.receiver_uuid << "\",\n";
			private_message_file << "\t\"body\": \"" << msg.body << "\"\n";
			private_message_file << "\t\"timestamp\": \"" << msg.timestamp << "\"\n";
			private_message_file << "}\n";
		}
		private_message_file.close();
		std::cout << "Saved " << _private_messages.size() << " private messages to " << PRIVATE_MESSAGEFILE_PATH << std::endl;
	}
}

void AEL::Engine::add(int fd)
{
	uuid_t uuid;
	char uuid_str[UUID_SIZE];
	uuid_generate_random(uuid);
    uuid_unparse(uuid, uuid_str);
	_anonymous_users.push_back(std::make_unique<User>("", std::string(uuid_str), fd));
	std::cout << "Registered anonymous user with uuid " << uuid_str << std::endl;
}

void AEL::Engine::remove(int fd)
{
	auto current_user = std::find_if(_anonymous_users.begin(), _anonymous_users.end(),
        [fd](const std::unique_ptr<User>& user) { return user->getFD() == fd; });
	if (current_user != _anonymous_users.end()) {
		std::cout << "Deleted anonymous user" << std::endl;
		_anonymous_users.erase(current_user);
	}
}

void AEL::Engine::USE(const std::string& team, const std::string& channel, const std::string& thread, User* user)
{
	auto team_obj = getTeamByUUID(team);
	if (!team_obj) {
		sendToClient(user, M4001(team));
		throw Minor("Team not found: " + team);
	}

	if (!user->isInTeam(team)) {
		sendToClient(user, M4005);
		throw Minor("User not subscribed to team: " + team);
	}

	if (!channel.empty()) {
		auto channel_obj = getChannelByUUID(channel);
		if (!channel_obj) {
			sendToClient(user, M4002(channel));
			throw Minor("Channel not found: " + channel);
		}
		
		if (channel_obj->getTeamUUID() != team) {
			sendToClient(user, M4002(channel));
			throw Minor("Channel does not belong to team: " + channel);
		}
	}

	if (!thread.empty()) {
		auto thread_obj = getThreadByUUID(thread);
		if (!thread_obj) {
			sendToClient(user, M4003(thread));
			throw Minor("Thread not found: " + thread);
		}
		
		if (thread_obj->getChannelUUID() != channel) {
			sendToClient(user, M4003(thread));
			throw Minor("Thread does not belong to channel: " + thread);
		}
	}

	user->setCurrentTeam(team);
	user->setCurrentChannel(channel);
	user->setCurrentThread(thread);
}
