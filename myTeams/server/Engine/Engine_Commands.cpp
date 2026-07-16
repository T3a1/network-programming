/*
** EPITECH PROJECT, 2026
** myteams
** File description:
** Engine_Commands
*/

#include "Engine.hpp"

bool AEL::Engine::LGI([[maybe_unused]] std::vector<std::string> command, [[maybe_unused]] User* user)
{
	std::cout << "LGI: Anonymous=" << _anonymous_users.size() << " Offline=" << _offline_users.size() 
	          << " Authenticated=" << _authenticated_users.size() << std::endl;
	
	if (command.size() == 1) {
		sendToClient(user, M410("Invalid LGI command"));
		throw Minor("Invalid LGI command");
	}

	auto current_auth = std::find_if(_authenticated_users.begin(), _authenticated_users.end(),
		[user](const auto& u) { return u.get() == user; });
	auto target_auth = std::find_if(_authenticated_users.begin(), _authenticated_users.end(),
		[&](const auto& tmp_user) { return tmp_user->getUsername() == command[1]; });

	if (current_auth != _authenticated_users.end() && user->getUsername() == command[1]) {
		sendToClient(user, M320("Already authenticated"));
		return false;
	}

	if (target_auth != _authenticated_users.end()) {
		if (current_auth != _authenticated_users.end() && current_auth->get() != target_auth->get()) {
			logoutCurrentSessionAndMoveToOffline(current_auth);
		}

		removeAnonymousByFD(user->getFD());

		auto mirrored_session = std::make_unique<User>(
			(*target_auth)->getUsername(),
			(*target_auth)->getUUID_str(),
			user->getFD()
		);
		for (const auto& team_uuid : (*target_auth)->getTeamUuids())
			mirrored_session->addTeamUuid(team_uuid);
		mirrored_session->setCurrentTeam((*target_auth)->getCurrentTeam());
		mirrored_session->setCurrentChannel((*target_auth)->getCurrentChannel());
		mirrored_session->setCurrentThread((*target_auth)->getCurrentThread());
		
		User* mirrored_session_ptr = mirrored_session.get();
		_authenticated_users.push_back(std::move(mirrored_session));

		std::string user_uuid = mirrored_session_ptr->getUUID_str();
		std::string user_name = mirrored_session_ptr->getUsername();
		std::string response = M220(user_uuid, user_name);
		sendToClient(mirrored_session_ptr, response);
		server_event_user_logged_in(user_uuid.c_str());

		notifyAuthenticatedSessionsExceptFD(mirrored_session_ptr->getFD(), response);
		return false;
	}

	auto offline_user = std::find_if(_offline_users.begin(), _offline_users.end(),
		[&](const auto& tmp_user) { return tmp_user->getUsername() == command[1]; });

	if (offline_user != _offline_users.end()) {
		if (current_auth != _authenticated_users.end()) {
			logoutCurrentSessionAndMoveToOffline(current_auth);
		}

		std::string user_uuid = (*offline_user)->getUUID_str();
		std::string user_name = (*offline_user)->getUsername();

		(*offline_user)->setFD(user->getFD());
		_authenticated_users.push_back(std::move(*offline_user));
		_offline_users.erase(offline_user);

		removeAnonymousByFD(user->getFD());

		std::string response = M220(user_uuid, user_name);
		sendToClient(_authenticated_users.back().get(), response);
		server_event_user_logged_in(user_uuid.c_str());

		notifyAuthenticatedSessionsExceptFD(_authenticated_users.back()->getFD(), response);
		return false;
	}

	uuid_t new_uuid;
	char new_uuid_str[UUID_SIZE];
	uuid_generate_random(new_uuid);
	uuid_unparse(new_uuid, new_uuid_str);

	if (current_auth != _authenticated_users.end()) {
		logoutCurrentSessionAndMoveToOffline(current_auth);
	}

	auto new_user = std::make_unique<User>(command[1], std::string(new_uuid_str), user->getFD());
	_authenticated_users.push_back(std::move(new_user));

	removeAnonymousByFD(user->getFD());

	std::string response = M220(std::string(new_uuid_str), command[1]);
	sendToClient(_authenticated_users.back().get(), response);
	server_event_user_created(new_uuid_str, command[1].c_str());
	server_event_user_logged_in(new_uuid_str);

	notifyAuthenticatedSessionsExceptFD(_authenticated_users.back()->getFD(), response);
	return false;
}

bool AEL::Engine::LGO([[maybe_unused]] std::vector<std::string> command, [[maybe_unused]] User* user)
{
	auto anon_user = std::find_if(_anonymous_users.begin(), _anonymous_users.end(),
		[user](const auto& u) { return u.get() == user; });
    
	if (anon_user != _anonymous_users.end()) {
		std::string user_uuid = user->getUUID_str();
		std::string user_name = user->getUsername();
		std::string response = M221(user_uuid, user_name);
		sendToClient(user, response);
		std::cout << "Anonymous user logged out and disconnected\n";
		server_event_user_logged_out(user->getUUID_str().c_str());
		return true;
	}
    
	auto auth_user = std::find_if(_authenticated_users.begin(), _authenticated_users.end(),
		[user](const auto& u) { return u.get() == user; });
    
	if (auth_user != _authenticated_users.end()) {
		std::string user_uuid = (*auth_user)->getUUID_str();
		std::string user_name = (*auth_user)->getUsername();
		std::string response = M221(user_uuid, user_name);
		sendToClient(user, response);

		logoutCurrentSessionAndMoveToOffline(auth_user);
		std::cout << "User " << user_name << " logged out\n";
		notifyAuthenticatedSessions(response);
		return true;
	}
    
	return false;
}

bool AEL::Engine::CRT([[maybe_unused]] std::vector<std::string> command, [[maybe_unused]] User* user)
{
	auto auth_user = std::find_if(_authenticated_users.begin(), _authenticated_users.end(),
		[user](const auto& u) { return u.get() == user; });
	if (auth_user == _authenticated_users.end()) {
		sendToClient(user, M4005);
		throw Minor("User not authenticated");
	}

	if (command.size() != 3) {
		sendToClient(user, M410("Invalid CRT command"));
		std::cout << "Command size: " << command.size() << std::endl;
		throw Minor("Invalid CRT command");
	}

	if (command[1].length() > MAX_NAME_LENGTH) {
		sendToClient(user, M410("Team name exceeds maximum length"));
		throw Minor("Team name exceeds maximum length");
	}

	if (command[2].length() > MAX_DESCRIPTION_LENGTH) {
		sendToClient(user, M410("Team description exceeds maximum length"));
		throw Minor("Team description exceeds maximum length");
	}

	for (const auto& team : _teams) {
		if (team->getName() == command[1]) {
			sendToClient(user, M230);
			throw Minor("Team already exists: " + command[1]);
		}
	}

	std::shared_ptr<Team> new_team = std::make_shared<Team>(command[1], command[2]);
	std::string team_uuid = new_team->getUUID_str();

	server_event_team_created(team_uuid.c_str(), new_team->getName().c_str(), user->getUUID_str().c_str());
	_teams.push_back(new_team);
	std::string response = M2300(new_team->getUUID_str(), new_team->getName(), new_team->getDescription());
	sendToClient(user, response);
	return false;
}

bool AEL::Engine::CRC([[maybe_unused]] std::vector<std::string> command, [[maybe_unused]] User* user)
{
	auto auth_user = std::find_if(_authenticated_users.begin(), _authenticated_users.end(),
		[user](const auto& u) { return u.get() == user; });
	if (auth_user == _authenticated_users.end()) {
		sendToClient(user, M4005);
		throw Minor("User not authenticated");
	}

	if (command.size() != 3) {
		sendToClient(user, M410("Invalid CRC command"));
		throw Minor("Invalid CRC command");
	}

	if (command[1].length() > MAX_NAME_LENGTH) {
		sendToClient(user, M410("Channel name exceeds maximum length"));
		throw Minor("Channel name exceeds maximum length");
	}

	if (command[2].length() > MAX_DESCRIPTION_LENGTH) {
		sendToClient(user, M410("Channel description exceeds maximum length"));
		throw Minor("Channel description exceeds maximum length");
	}

	if (!user->hasTeamContext()) {
		sendToClient(user, M411("No team context set"));
		throw Minor("No team context set");
	}

	std::string team_uuid = user->getCurrentTeam();
	if (!user->isInTeam(team_uuid)) {
		sendToClient(user, M400("User not subscribed to team"));
		throw Minor("User not subscribed to team");
	}

	for (const auto& channel : _channels) {
		if (channel->getName() == command[1]) {
			sendToClient(user, M230);
			throw Minor("Channel already exists: " + command[1]);
		}
	}

	std::shared_ptr<Channel> new_channel = std::make_shared<Channel>(command[1], command[2], team_uuid);
	std::string channel_uuid = new_channel->getUUID_str();
	std::string channel_name = new_channel->getName();
	std::string channel_desc = new_channel->getDescription();

	_channels.push_back(new_channel);
	server_event_channel_created(team_uuid.c_str(), channel_uuid.c_str(), channel_name.c_str());

	std::cout << "Channel " << command[1] << " created in team " << team_uuid << std::endl;
	std::string response = M2301(channel_uuid, channel_name, channel_desc);

	notifyTeamUsers(team_uuid, response);

	return false;
}

bool AEL::Engine::CRH([[maybe_unused]] std::vector<std::string> command, [[maybe_unused]] User* user)
{
	auto auth_user = std::find_if(_authenticated_users.begin(), _authenticated_users.end(),
		[user](const auto& u) { return u.get() == user; });
	if (auth_user == _authenticated_users.end()) {
		sendToClient(user, M4005);
		throw Minor("User not authenticated");
	}

	if (command.size() != 3) {
		sendToClient(user, M410("Invalid CRH command"));
		throw Minor("Invalid CRH command");
	}

	if (command[1].length() > MAX_NAME_LENGTH) {
		sendToClient(user, M410("Thread title exceeds maximum length"));
		throw Minor("Thread title exceeds maximum length");
	}

	if (command[2].length() > MAX_BODY_LENGTH) {
		sendToClient(user, M410("Thread body exceeds maximum length"));
		throw Minor("Thread body exceeds maximum length");
	}

	if (!user->hasChannelContext()) {
		sendToClient(user, M411("No channel context set"));
		throw Minor("No channel context set");
	}

	std::string team_uuid = user->getCurrentTeam();
	if (!user->isInTeam(team_uuid)) {
		sendToClient(user, M400("User not subscribed to team"));
		throw Minor("User not subscribed to team");
	}

	std::string channel_uuid = user->getCurrentChannel();

	for (const auto& thread : _threads) {
		if (thread->getName() == command[1]) {
			sendToClient(user, M230);
			throw Minor("Thread already exists: " + command[1]);
		}
	}

	std::shared_ptr<Thread> new_thread = std::make_shared<Thread>(command[1], command[2], channel_uuid, user->getUUID_str());
	std::string thread_uuid = new_thread->getUUID_str();
	std::string thread_title = new_thread->getName();
	std::string thread_body = new_thread->getDescription();
	std::time_t thread_timestamp = new_thread->getTimestamp();

	_threads.push_back(new_thread);
	server_event_thread_created(channel_uuid.c_str(), thread_uuid.c_str(), user->getUUID_str().c_str(),
								  thread_title.c_str(), thread_body.c_str());

	std::cout << "Thread " << command[1] << " created in channel " << channel_uuid << std::endl;
	std::string response = M2302(thread_uuid, user->getUUID_str(), thread_timestamp, thread_title, thread_body);

	notifyTeamUsers(team_uuid, response);

	return false;
}

bool AEL::Engine::CRR([[maybe_unused]] std::vector<std::string> command, [[maybe_unused]] User* user)
{
	auto auth_user = std::find_if(_authenticated_users.begin(), _authenticated_users.end(),
		[user](const auto& u) { return u.get() == user; });
	if (auth_user == _authenticated_users.end()) {
		sendToClient(user, M4005);
		throw Minor("User not authenticated");
	}

	if (command.size() != 2) {
		sendToClient(user, M410("Invalid CRR command"));
		throw Minor("Invalid CRR command");
	}

	if (command[1].length() > MAX_BODY_LENGTH) {
		sendToClient(user, M410("Reply body exceeds maximum length"));
		throw Minor("Reply body exceeds maximum length");
	}

	if (!user->hasThreadContext()) {
		sendToClient(user, M411("No thread context set"));
		throw Minor("No thread context set");
	}

	std::string team_uuid = user->getCurrentTeam();
	if (!user->isInTeam(team_uuid)) {
		sendToClient(user, M400("User not subscribed to team"));
		throw Minor("User not subscribed to team");
	}

	std::string thread_uuid = user->getCurrentThread();
	std::shared_ptr<Comment> new_reply = std::make_shared<Comment>("", command[1], thread_uuid, user->getUUID_str());
	std::string reply_body = command[1];
	std::time_t reply_timestamp = new_reply->getTimestamp();

	_comments.push_back(new_reply);
	server_event_reply_created(thread_uuid.c_str(), user->getUUID_str().c_str(), reply_body.c_str());

	std::cout << "Reply created in thread " << thread_uuid << std::endl;
	std::string response = M2303(thread_uuid, user->getUUID_str(), reply_timestamp, reply_body);

	notifyTeamUsers(team_uuid, response);

	return false;
}

bool AEL::Engine::SND([[maybe_unused]] std::vector<std::string> command, [[maybe_unused]] User* user)
{
	auto auth_user = std::find_if(_authenticated_users.begin(), _authenticated_users.end(),
		[user](const auto& u) { return u.get() == user; });
	if (auth_user == _authenticated_users.end()) {
		sendToClient(user, M4005);
		throw Minor("User not authenticated");
	}

	if (command.size() < 3) {
		sendToClient(user, M410("Invalid SEND command"));
		throw Minor("Invalid SEND command");
	}

	User* receiver = getUserByUUID(command[1]);
	if (!receiver) {
		sendToClient(user, M4004(command[1]));
		throw Minor("User not found: " + command[1]);
	}

	if (command[2].length() > MAX_BODY_LENGTH) {
		sendToClient(user, M410("Message body exceeds maximum length"));
		throw Minor("Message body exceeds maximum length");
	}

	std::string message_body = command[2];
	for (size_t i = 3; i < command.size(); ++i) {
		message_body += " " + command[i];
	}

	Message msg;
	msg.sender_uuid = user->getUUID_str();
	msg.receiver_uuid = receiver->getUUID_str();
	msg.body = message_body;

	time_t now = time(nullptr);

	_private_messages.push_back(msg);
	server_event_private_message_sended(msg.sender_uuid.c_str(), msg.receiver_uuid.c_str(), msg.body.c_str());

	sendToClient(user, "1004 " + receiver->getUUID_str() + " " + std::to_string(now) + " \"" + message_body + "\"");
	sendToClient(receiver, "2000 " + user->getUUID_str() + " " + std::to_string(now) + " \"" + message_body + "\"");
	msg.timestamp = now;
	return false;
}

bool AEL::Engine::SUB([[maybe_unused]] std::vector<std::string> command, [[maybe_unused]] User* user)
{
	if (command.size() != 2) {
		sendToClient(user, M410("Invalid SUB command"));
		throw Minor("Invalid SUB command");
	}

	auto auth_user = std::find_if(_authenticated_users.begin(), _authenticated_users.end(),
		[user](const auto& u) { return u.get() == user; });
	if (auth_user == _authenticated_users.end()) {
		sendToClient(user, M530("User not authenticated"));
		throw Minor("User not authenticated");
	}

	std::shared_ptr<Team> team = getTeamByUUID(command[1]);
	if (!team) {
		sendToClient(user, M4001(command[1]));
		throw Minor("Team not found: " + command[1]);
	}

	if (user->isInTeam(command[1])) {
		sendToClient(user, M413("User already subscribed to team"));
		throw Minor("User already subscribed to team");
	}

	std::string current_uuid = user->getUUID_str();
	for (const auto& auth_session : _authenticated_users) {
		if (auth_session->getUUID_str() == current_uuid && !auth_session->isInTeam(command[1]))
			auth_session->addTeamUuid(command[1]);
	}
	for (const auto& offline_user : _offline_users) {
		if (offline_user->getUUID_str() == current_uuid && !offline_user->isInTeam(command[1]))
			offline_user->addTeamUuid(command[1]);
	}

	std::string response = M2400(user->getUUID_str(), command[1]);
	server_event_user_subscribed(command[1].c_str(), user->getUUID_str().c_str());
	notifyTeamUsers(command[1], response);

	std::cout << "User " << user->getUsername() << " subscribed to team " << command[1] << std::endl;
	return false;
}

bool AEL::Engine::UNS([[maybe_unused]] std::vector<std::string> command, [[maybe_unused]] User* user)
{
	if (command.size() != 2) {
		sendToClient(user, M410("Invalid UNSUB command"));
		throw Minor("Invalid UNSUB command");
	}

	auto auth_user = std::find_if(_authenticated_users.begin(), _authenticated_users.end(),
		[user](const auto& u) { return u.get() == user; });
	if (auth_user == _authenticated_users.end()) {
		sendToClient(user, M530("User not authenticated"));
		throw Minor("User not authenticated");
	}

	std::shared_ptr<Team> team = getTeamByUUID(command[1]);
	if (!team) {
		sendToClient(user, M4001(command[1]));
		throw Minor("Team not found: " + command[1]);
	}

	if (!user->isInTeam(command[1])) {
		sendToClient(user, M413("User not subscribed to team"));
		throw Minor("User not subscribed to team");
	}

	if (user->getCurrentTeam() == command[1]) {
		sendToClient(user, M413("Cannot unsubscribe from current team context"));
		throw Minor("Cannot unsubscribe from current team context");
	}

	std::string response = M2401(user->getUUID_str(), command[1]);
	notifyTeamUsers(command[1], response);
	std::string current_uuid = user->getUUID_str();
	for (const auto& auth_session : _authenticated_users) {
		if (auth_session->getUUID_str() == current_uuid)
			auth_session->removeTeamUuid(command[1]);
	}
	for (const auto& offline_user : _offline_users) {
		if (offline_user->getUUID_str() == current_uuid)
			offline_user->removeTeamUuid(command[1]);
	}
	server_event_user_unsubscribed(command[1].c_str(), user->getUUID_str().c_str());

	std::cout << "User " << user->getUsername() << " unsubscribed from team " << command[1] << std::endl;
	return false;
}

bool AEL::Engine::USS([[maybe_unused]] std::vector<std::string> command, [[maybe_unused]] User* user)
{
	auto auth_user = std::find_if(_authenticated_users.begin(), _authenticated_users.end(),
		[user](const auto& u) { return u.get() == user; });
	if (auth_user == _authenticated_users.end()) {
		sendToClient(user, M4005);
		throw Minor("User not authenticated");
	}

	std::string united_response;
	std::vector<std::string> listed_uuids;

	for (const auto& auth_user : _authenticated_users) {
		if (std::find(listed_uuids.begin(), listed_uuids.end(), auth_user->getUUID_str()) != listed_uuids.end())
			continue;
		listed_uuids.push_back(auth_user->getUUID_str());
		united_response += M2100(auth_user->getUUID_str(), auth_user->getUsername(), "1") + "\n";
	}

	for (const auto& offline_user : _offline_users) {
		if (std::find(listed_uuids.begin(), listed_uuids.end(), offline_user->getUUID_str()) != listed_uuids.end())
			continue;
		listed_uuids.push_back(offline_user->getUUID_str());
		united_response += M2100(offline_user->getUUID_str(), offline_user->getUsername(), "0") + "\n";
	}

	if (!united_response.empty()) {
		united_response.pop_back();
		sendToClient(user, united_response);
	}

	return false;
}

bool AEL::Engine::USR([[maybe_unused]] std::vector<std::string> command, [[maybe_unused]] User* user)
{
	if (command.size() != 2) {
		sendToClient(user, M410("Invalid USR command"));
		throw Minor("Invalid USR command");
	}

	auto auth_user = std::find_if(_authenticated_users.begin(), _authenticated_users.end(),
		[user](const auto& u) { return u.get() == user; });
	if (auth_user == _authenticated_users.end()) {
		sendToClient(user, M4005);
		throw Minor("User not authenticated");
	}

	User* target_user = getUserByUUID(command[1]);
	if (!target_user) {
		sendToClient(user, M4004(command[1]));
		throw Minor("User not found: " + command[1]);
	}

	auto is_authenticated = std::find_if(_authenticated_users.begin(), _authenticated_users.end(),
		[target_user](const auto& u) { return u.get() == target_user; });
	
	int status = (is_authenticated != _authenticated_users.end()) ? 1 : 0;
	std::string response = M2200(target_user->getUUID_str(), target_user->getUsername(), std::to_string(status));
	sendToClient(user, response);

	return false;
}

bool AEL::Engine::SBD([[maybe_unused]] std::vector<std::string> command, [[maybe_unused]] User* user)
{
	auto auth_user = std::find_if(_authenticated_users.begin(), _authenticated_users.end(),
		[user](const auto& u) { return u.get() == user; });
	if (auth_user == _authenticated_users.end()) {
		sendToClient(user, M4005);
		throw Minor("User not authenticated");
	}

	if (command.size() == 1) {
		std::string batch_response;

		for (const auto& team_uuid : user->getTeamUuids()) {
			std::shared_ptr<Team> team = getTeamByUUID(team_uuid);
			std::cout << " - Team UUID: " << team_uuid << std::endl;
			if (team)
				batch_response += "2101 " + team->getUUID_str() + " \"" + team->getName() + "\" \"" + team->getDescription() + "\"\n";
		}

		if (!batch_response.empty()) {
			batch_response.pop_back();
			sendToClient(user, batch_response);
		} else
			sendToClient(user, M2101("x", "None", "Subscribed_to_no_teams"));
		return false;
	}

	if (command.size() == 2) {
		std::shared_ptr<Team> team = getTeamByUUID(command[1]);
		if (!team) {
			sendToClient(user, M4001(command[1]));
			throw Minor("Team not found: " + command[1]);
		}

		std::string batch_response;
		std::vector<std::string> listed_uuids;

		for (const auto& auth_user : _authenticated_users) {
			if (!auth_user->isInTeam(command[1]))
				continue;
			if (std::find(listed_uuids.begin(), listed_uuids.end(), auth_user->getUUID_str()) != listed_uuids.end())
				continue;
			listed_uuids.push_back(auth_user->getUUID_str());
			batch_response += M2100(auth_user->getUUID_str(), auth_user->getUsername(), "1") + "\n";
		}

		for (const auto& offline_user : _offline_users) {
			if (!offline_user->isInTeam(command[1]))
				continue;
			if (std::find(listed_uuids.begin(), listed_uuids.end(), offline_user->getUUID_str()) != listed_uuids.end())
				continue;
			listed_uuids.push_back(offline_user->getUUID_str());
			batch_response += M2100(offline_user->getUUID_str(), offline_user->getUsername(), "0") + "\n";
		}

		if (!batch_response.empty()) {
			batch_response.pop_back();
			sendToClient(user, batch_response);
		} else
			sendToClient(user, M2100("0", "No_users_in_team", "0"));
		return false;
	}

	sendToClient(user, M410("Invalid SBD command"));
	throw Minor("Invalid SBD command");
}

bool AEL::Engine::MSG([[maybe_unused]] std::vector<std::string> command, [[maybe_unused]] User* user)
{
	auto auth_user = std::find_if(_authenticated_users.begin(), _authenticated_users.end(),
		[user](const auto& u) { return u.get() == user; });
	if (auth_user == _authenticated_users.end()) {
		sendToClient(user, M4005);
		throw Minor("User not authenticated");
	}

	if (command.size() == 1) {
		sendToClient(user, M410("Invalid MSG command"));
		throw Minor("Invalid MSG command");
	}

	auto recipient = std::find_if(_authenticated_users.begin(), _authenticated_users.end(),
		[&](const auto& u) { return u->getUUID_str() == command[1]; });
	
	if (recipient == _authenticated_users.end()) {
		recipient = std::find_if(_offline_users.begin(), _offline_users.end(),
			[&](const auto& u) { return u->getUUID_str() == command[1]; });
		
		if (recipient == _offline_users.end()) {
			sendToClient(user, M4002(command[1]));
			throw Minor("User not found: " + command[1]);
		}
	}

	std::string batch_response;
	
	for (const auto& msg : _private_messages) {
		if ((msg.sender_uuid == command[1] && msg.receiver_uuid == user->getUUID_str()) ||
			(msg.receiver_uuid == command[1] && msg.sender_uuid == user->getUUID_str())) {
			batch_response += M2105(msg.sender_uuid, msg.timestamp, msg.body) + "\n";
		}
	}
	
	if (!batch_response.empty()) {
		batch_response.pop_back();
		sendToClient(user, batch_response);
	}
	return false;
}

bool AEL::Engine::LST([[maybe_unused]] std::vector<std::string> command, [[maybe_unused]] User* user)
{
	auto auth_user = std::find_if(_authenticated_users.begin(), _authenticated_users.end(),
		[user](const auto& u) { return u.get() == user; });
	if (auth_user == _authenticated_users.end()) {
		sendToClient(user, M4005);
		throw Minor("User not authenticated");
	}

	if (!user->hasTeamContext()) {
		std::string batch_response;
		for (const auto& team : _teams) {
			batch_response += M2101(team->getUUID_str(), team->getName(), team->getDescription()) + "\n";
		}
		if (!batch_response.empty()) {
			batch_response.pop_back();
			sendToClient(user, batch_response);
		} else {
			sendToClient(user, M410("No teams to list"));
		}
		return false;
	}

	if (!user->hasChannelContext()) {
		std::string team_uuid = user->getCurrentTeam();
		std::string batch_response;
		for (const auto& channel : _channels) {
			if (channel->getTeamUUID() == team_uuid) {
				batch_response += M2102(channel->getUUID_str(), channel->getName(), channel->getDescription()) + "\n";
			}
		}
		if (!batch_response.empty()) {
			batch_response.pop_back();
			sendToClient(user, batch_response);
		} else {
			sendToClient(user, M410("No channels to list"));
		}
		return false;
	}

	if (!user->hasThreadContext()) {
		std::string channel_uuid = user->getCurrentChannel();
		std::string batch_response;
		for (const auto& thread : _threads) {
			if (thread->getChannelUUID() == channel_uuid) {
				batch_response += M2103(thread->getUUID_str(), thread->getAuthorUUID(), thread->getTimestamp(), thread->getName(), thread->getDescription()) + "\n";
			}
		}
		if (!batch_response.empty()) {
			batch_response.pop_back();
			sendToClient(user, batch_response);
		} else {
			sendToClient(user, M410("No threads to list"));
		}
		return false;
	}

	std::string thread_uuid = user->getCurrentThread();
	std::string batch_response;
	for (const auto& comment : _comments) {
		if (comment->getThreadUUID() == thread_uuid) {
			batch_response += M2104(comment->getThreadUUID(), comment->getAuthorUUID(), comment->getTimestamp(), comment->getDescription()) + "\n";
		}
	}
	if (!batch_response.empty()) {
		batch_response.pop_back();
		sendToClient(user, batch_response);
	} else {
		sendToClient(user, M410("No replies to list"));
	}
	return false;
}

bool AEL::Engine::INF([[maybe_unused]] std::vector<std::string> command, [[maybe_unused]] User* user)
{
	auto auth_user = std::find_if(_authenticated_users.begin(), _authenticated_users.end(),
		[user](const auto& u) { return u.get() == user; });
	if (auth_user == _authenticated_users.end()) {
		sendToClient(user, M4005);
		throw Minor("User not authenticated");
	}

	if (command.size() != 1) {
		sendToClient(user, M410("Invalid info command"));
		throw Minor("Invalid info command");
	}

	std::string cmd = command[0];

	if (cmd == "IFU") {
		std::string response = M2200(user->getUUID_str(), user->getUsername(), "1");
		sendToClient(user, response);
		return false;
	}

	if (cmd == "IFT") {
		if (!user->hasTeamContext()) {
			sendToClient(user, M411("No team context set"));
			throw Minor("No team context set");
		}

		std::string team_uuid = user->getCurrentTeam();
		std::shared_ptr<Team> team = getTeamByUUID(team_uuid);
		
		if (!team) {
			sendToClient(user, M4001(team_uuid));
			throw Minor("Team not found: " + team_uuid);
		}

		std::string response = M2201(team->getUUID_str(), team->getName(), team->getDescription());
		sendToClient(user, response);
		return false;
	}

	if (cmd == "IFC") {
		if (!user->hasChannelContext()) {
			sendToClient(user, M411("No channel context set"));
			throw Minor("No channel context set");
		}

		std::string channel_uuid = user->getCurrentChannel();
		std::shared_ptr<Channel> channel = getChannelByUUID(channel_uuid);
		
		if (!channel) {
			sendToClient(user, M4002(channel_uuid));
			throw Minor("Channel not found: " + channel_uuid);
		}

		std::string response = M2202(channel->getUUID_str(), channel->getName(), channel->getDescription());
		sendToClient(user, response);
		return false;
	}

	if (cmd == "IFH") {
		if (!user->hasThreadContext()) {
			sendToClient(user, M411("No thread context set"));
			throw Minor("No thread context set");
		}

		std::string thread_uuid = user->getCurrentThread();
		std::shared_ptr<Thread> thread = getThreadByUUID(thread_uuid);
		
		if (!thread) {
			sendToClient(user, M4003(thread_uuid));
			throw Minor("Thread not found: " + thread_uuid);
		}

		std::string response = M2203(thread->getUUID_str(), thread->getAuthorUUID(), thread->getTimestamp(), thread->getName(), thread->getDescription());
		sendToClient(user, response);
		return false;
	}

	return false;
}
