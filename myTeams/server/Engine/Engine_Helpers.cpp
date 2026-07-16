/*
** EPITECH PROJECT, 2026
** myteams
** File description:
** Engine_Helpers
*/

#include "Engine.hpp"

std::shared_ptr<AEL::Team> AEL::Engine::getTeamByUUID(const std::string& uuid)
{
	auto team = std::find_if(_teams.begin(), _teams.end(),
		[&uuid](const std::shared_ptr<Team>& t) { return t->getUUID_str() == uuid; });
	if (team != _teams.end())
		return *team;
	return nullptr;
}

std::shared_ptr<AEL::Channel> AEL::Engine::getChannelByUUID(const std::string& uuid)
{
	auto channel = std::find_if(_channels.begin(), _channels.end(),
		[&uuid](const std::shared_ptr<Channel>& c) { return c->getUUID_str() == uuid; });
	if (channel != _channels.end())
		return *channel;
	return nullptr;
}

std::shared_ptr<AEL::Thread> AEL::Engine::getThreadByUUID(const std::string& uuid)
{
	auto thread = std::find_if(_threads.begin(), _threads.end(),
		[&uuid](const std::shared_ptr<Thread>& t) { return t->getUUID_str() == uuid; });
	if (thread != _threads.end())
		return *thread;
	return nullptr;
}

std::shared_ptr<AEL::Comment> AEL::Engine::getCommentByUUID(const std::string& uuid)
{
	auto comment = std::find_if(_comments.begin(), _comments.end(),
		[&uuid](const std::shared_ptr<Comment>& c) { return c->getUUID_str() == uuid; });
	if (comment != _comments.end())
		return *comment;
	return nullptr;
}

AEL::User* AEL::Engine::getUserByUsername(const std::string& username)
{
	auto auth_user = std::find_if(_authenticated_users.begin(), _authenticated_users.end(),
		[&username](const std::unique_ptr<User>& u) { return u->getUsername() == username; });
	if (auth_user != _authenticated_users.end())
		return auth_user->get();

	auto offline_user = std::find_if(_offline_users.begin(), _offline_users.end(),
		[&username](const std::unique_ptr<User>& u) { return u->getUsername() == username; });
	if (offline_user != _offline_users.end())
		return offline_user->get();

	return nullptr;
}

AEL::User* AEL::Engine::getUserByFD(int fd)
{
	auto auth_user = std::find_if(_authenticated_users.begin(), _authenticated_users.end(),
        [fd](const std::unique_ptr<User>& u) { return u->getFD() == fd; });
	if (auth_user != _authenticated_users.end())
		return auth_user->get();

	auto anon_user = std::find_if(_anonymous_users.begin(), _anonymous_users.end(),
        [fd](const std::unique_ptr<User>& u) { return u->getFD() == fd; });
	if (anon_user != _anonymous_users.end())
		return anon_user->get();

	auto offline_user = std::find_if(_offline_users.begin(), _offline_users.end(),
        [fd](const std::unique_ptr<User>& u) { return u->getFD() == fd; });
	if (offline_user != _offline_users.end())
		return offline_user->get();

	return nullptr;
}

AEL::User* AEL::Engine::getUserByUUID(const std::string& uuid)
{
	auto auth_user = std::find_if(_authenticated_users.begin(), _authenticated_users.end(),
		[&uuid](const std::unique_ptr<User>& u) { return u->getUUID_str() == uuid; });
	if (auth_user != _authenticated_users.end())
		return auth_user->get();

	auto anon_user = std::find_if(_anonymous_users.begin(), _anonymous_users.end(),
		[&uuid](const std::unique_ptr<User>& u) { return u->getUUID_str() == uuid; });
	if (anon_user != _anonymous_users.end())
		return anon_user->get();

	auto offline_user = std::find_if(_offline_users.begin(), _offline_users.end(),
		[&uuid](const std::unique_ptr<User>& u) { return u->getUUID_str() == uuid; });
	if (offline_user != _offline_users.end())
		return offline_user->get();

	return nullptr;
}

void AEL::Engine::moveToOfflineIfLastSession(User* session_user)
{
	auto another_session = std::find_if(_authenticated_users.begin(), _authenticated_users.end(),
		[&](const auto& candidate) {
			return candidate.get() != session_user &&
				candidate->getUUID_str() == session_user->getUUID_str();
		});
	if (another_session == _authenticated_users.end()) {
		// No other sessions, mark user as offline and prepare for move
		session_user->setFD(-1);
		
	}
	server_event_user_logged_out(session_user->getUUID_str().c_str());
}

void AEL::Engine::notifyTeamUsers(const std::string& team_uuid, const std::string& message)
{
	for (const auto& user : _authenticated_users) {
		if (user->isInTeam(team_uuid)) {
			sendToClient(user.get(), message);
		}
	}
}

void AEL::Engine::logoutCurrentSessionAndMoveToOffline(
	std::vector<std::unique_ptr<User>>::iterator session_it)
{
	moveToOfflineIfLastSession(session_it->get());
	
	bool has_another_session = std::any_of(_authenticated_users.begin(), _authenticated_users.end(),
		[&](const auto& candidate) {
			return candidate.get() != session_it->get() && 
				candidate->getUUID_str() == session_it->get()->getUUID_str();
		});
	
	// server_event_user_logged_out(session_it->get()->getUUID_str().c_str());
	if (!has_another_session) {
		_offline_users.push_back(std::move(*session_it));
	}
	_authenticated_users.erase(session_it);
}

void AEL::Engine::removeAnonymousByFD(int fd)
{
	auto anon_user = std::find_if(_anonymous_users.begin(), _anonymous_users.end(),
		[fd](const auto& user) { return user->getFD() == fd; });
	if (anon_user != _anonymous_users.end())
		_anonymous_users.erase(anon_user);
}

void AEL::Engine::notifyAuthenticatedUsers(const std::string& exclude_uuid, const std::string& response)
{
	std::vector<std::string> notified_uuids;
	for (const auto& tmp_user : _authenticated_users) {
		if (tmp_user->getUUID_str() == exclude_uuid)
			continue;
		if (std::find(notified_uuids.begin(), notified_uuids.end(), tmp_user->getUUID_str()) != notified_uuids.end())
			continue;
		notified_uuids.push_back(tmp_user->getUUID_str());
		sendToClient(tmp_user.get(), response);
	}
}

void AEL::Engine::notifyAuthenticatedSessions(const std::string& response)
{
	for (const auto& tmp_user : _authenticated_users)
		sendToClient(tmp_user.get(), response);
}

void AEL::Engine::notifyAuthenticatedSessionsExceptFD(int excluded_fd, const std::string& response)
{
	for (const auto& tmp_user : _authenticated_users) {
		if (tmp_user->getFD() == excluded_fd)
			continue;
		sendToClient(tmp_user.get(), response);
	}
}
