/*
** EPITECH PROJECT, 2026
** G-NWP-400-LIL-4-1-myteams-2
** File description:
** Parsing
*/

#include "Parsing.hpp"
#include <filesystem>

Parsing::Parsing(std::string filePath, std::string teamFilePath, std::string channelFilePath, std::string threadFilePath, std::string commentFilePath, std::string privateMessageFilePath)
{
    _users = std::vector<User>();
    _teams = std::vector<Team>();
    _userFilePath = filePath;
    _teamFilePath = teamFilePath;
    _channelFilePath = channelFilePath;
    _threadFilePath = threadFilePath;
    _commentFilePath = commentFilePath;
    _privateMessageFilePath = privateMessageFilePath;
}

Parsing::~Parsing()
{
}

const std::vector<Parsing::User>& Parsing::getUsers()
{
    std::ifstream file(_userFilePath);

    if (!checkBrackets(file))
        throw std::runtime_error("Invalid user file format: missing brackets");
    
    file.close();

    file.open(_userFilePath);

    if (!file.is_open())
        throw std::runtime_error("Unable to open user file");

    _users.clear();

    bool inUserBlock = false;
    User currentUser;

    for (std::string line; std::getline(file, line);) {
        std::string cleanedLine = cleanLine(line);

        if (cleanedLine.empty())
            continue;

        if (cleanedLine.front() == '{') {
            currentUser = User();
            inUserBlock = true;
            continue;
        }

        if (inUserBlock && cleanedLine.front() == '}') {
            _users.push_back(currentUser);
            inUserBlock = false;
            continue;
        }

        if (!inUserBlock)
            continue;

        std::size_t colonPos = cleanedLine.find(':');

        if (colonPos == std::string::npos)
            continue;

        std::string key = trim(cleanedLine.substr(0, colonPos));
        std::string value = trim(cleanedLine.substr(colonPos + 1));

        if (!value.empty() && value.back() == ',')
            value.pop_back();
        if (!key.empty() && key.front() == '"' && key.back() == '"')
            key = key.substr(1, key.size() - 2);
        if (!value.empty() && value.front() == '"' && value.back() == '"')
            value = value.substr(1, value.size() - 2);

        if (key == "name")
            currentUser.name = value;
        else if (key == "userUuid")
            currentUser.userUuid = value;
    }

    for (const auto& user : _users) {
        if (user.name.empty() || user.userUuid.empty())
            throw std::runtime_error("Invalid user file format: missing required fields (name and userUuid)");
    }

    file.close();
    return _users;
}

std::string Parsing::cleanLine(const std::string& line) const
{
    const std::string whitespace = " \t\r\n";
    const std::size_t first = line.find_first_not_of(whitespace);

    if (first == std::string::npos)
        return "";

    const std::size_t last = line.find_last_not_of(whitespace);
    return line.substr(first, last - first + 1);
}

std::string Parsing::trim(const std::string& line) const
{
    const std::string whitespace = " \t\r\n";
    const std::size_t first = line.find_first_not_of(whitespace);

    if (first == std::string::npos)
        return "";

    const std::size_t last = line.find_last_not_of(whitespace);
    return line.substr(first, last - first + 1);
}

bool Parsing::checkBrackets(std::ifstream& file) const
{
    char c = '\0';
    char previousBrace = '\0';
    int bracketPositivity = 0;

    if (!file.is_open())
        throw std::runtime_error("Unable to open persistence file");

    while (file.get(c)) {
        if (c != '{' && c != '}')
            continue;
        if (c == '{') {
            if (previousBrace == '{')
                return false;
            bracketPositivity++;
        } else {
            if (previousBrace == '}')
                return false;
            bracketPositivity--;
            if (bracketPositivity < 0)
                return false;
        }
        previousBrace = c;
    }
    return bracketPositivity == 0;
}

const std::vector<Parsing::Team>& Parsing::getTeams()
{
    if (!std::filesystem::exists(_teamFilePath)) {
        _teams.clear();
        return _teams;
    }

    std::ifstream file(_teamFilePath);

    if (!checkBrackets(file))
        throw std::runtime_error("Invalid team file format: missing brackets");
    file.close();

    file.open(_teamFilePath);

    bool inTeamBlock = false;
    Team currentTeam;

    for (std::string line; std::getline(file, line);) {
        std::string cleanedLine = cleanLine(line);

        if (cleanedLine.empty())
            continue;

        if (cleanedLine.front() == '{') {
            currentTeam = Team();
            inTeamBlock = true;
            continue;
        }

        if (inTeamBlock && cleanedLine.front() == '}') {
            getChannels(currentTeam);
            _teams.push_back(currentTeam);
            inTeamBlock = false;
            continue;
        }

        if (!inTeamBlock)
            continue;

        std::size_t colonPos = cleanedLine.find(':');

        if (colonPos == std::string::npos)
            continue;

        std::string key = trim(cleanedLine.substr(0, colonPos));
        std::string value = trim(cleanedLine.substr(colonPos + 1));

        if (!value.empty() && value.back() == ',')
            value.pop_back();
        if (!key.empty() && key.front() == '"' && key.back() == '"')
            key = key.substr(1, key.size() - 2);
        if (!value.empty() && value.front() == '"' && value.back() == '"')
            value = value.substr(1, value.size() - 2);

        if (key == "name")
            currentTeam.name = value;
        else if (key == "teamUuid")
            currentTeam.teamUuid = value;
        else if (key == "description")
            currentTeam.description = value;
        else if (key == "usersUuid") {
            if (!value.empty() && value.front() == '[' && value.back() == ']')
                value = value.substr(1, value.size() - 2);
            
            if (!value.empty()) {
                std::size_t start = 0;
                std::size_t end = value.find(',');
                
                while (end != std::string::npos) {
                    std::string uuid = trim(value.substr(start, end - start));

                     if (uuid.front() == '"' && uuid.back() == '"')
                        uuid = uuid.substr(1, uuid.size() - 2);

                    if (!uuid.empty())
                        currentTeam.authorizedUsers.push_back(User{"", uuid});
                    
                    start = end + 1;
                    end = value.find(',', start);
                }
                
                std::string uuid = trim(value.substr(start));
                
                if (uuid.front() == '"' && uuid.back() == '"')
                    uuid = uuid.substr(1, uuid.size() - 2);
                if (!uuid.empty())
                    currentTeam.authorizedUsers.push_back(User{"", uuid});
            }
        }
    }

    file.close();

    for (const auto& team : _teams) {
        if (team.name.empty() || team.teamUuid.empty())
            throw std::runtime_error("Invalid team file format: missing required fields (name and teamUuid)");
    }
    return _teams;
}

void Parsing::getChannels(Team& team)
{
    if (!std::filesystem::exists(_channelFilePath))
        return;

    std::ifstream file(_channelFilePath);

    if (!checkBrackets(file))
        throw std::runtime_error("Invalid channel file format: missing brackets");
    file.close();

    file.open(_channelFilePath);

    bool inChannelBlock = false;
    Channel currentChannel;

    for (std::string line; std::getline(file, line);) {
        std::string cleanedLine = cleanLine(line);

        if (cleanedLine.empty())
            continue;

        if (cleanedLine.front() == '{') {
            currentChannel = Channel();
            inChannelBlock = true;
            continue;
        }

        if (inChannelBlock && cleanedLine.front() == '}') {
            if (team.teamUuid == currentChannel.teamUuid) {
                getThreads(currentChannel);
                team.channels.push_back(currentChannel);
            }
            inChannelBlock = false;
            continue;
        }

        if (!inChannelBlock)
            continue;

        std::size_t colonPos = cleanedLine.find(':');

        if (colonPos == std::string::npos)
            continue;

        std::string key = trim(cleanedLine.substr(0, colonPos));
        std::string value = trim(cleanedLine.substr(colonPos + 1));

        if (!value.empty() && value.back() == ',')
            value.pop_back();
        if (!key.empty() && key.front() == '"' && key.back() == '"')
            key = key.substr(1, key.size() - 2);
        if (!value.empty() && value.front() == '"' && value.back() == '"')
            value = value.substr(1, value.size() - 2);
        if (key == "name")
            currentChannel.name = value;
        else if (key == "channelUuid")
            currentChannel.channelUuid = value;
        else if (key == "teamUuid")
            currentChannel.teamUuid = value;
        else if (key == "description")
            currentChannel.description = value;
    }

    file.close();

    for (const auto& channel : team.channels) {
        if (channel.name.empty() || channel.channelUuid.empty() || channel.teamUuid.empty())
            throw std::runtime_error("Invalid channel file format: missing required fields (name, channelUuid and teamUuid)");
    }
}

void Parsing::getThreads(Channel& channel)
{
    if (!std::filesystem::exists(_threadFilePath))
        return;

    std::ifstream file(_threadFilePath);

    if (!checkBrackets(file))
        throw std::runtime_error("Invalid thread file format: missing brackets");
    file.close();

    file.open(_threadFilePath);

    bool inThreadBlock = false;
    Threads currentThread;

    for (std::string line; std::getline(file, line);) {
        std::string cleanedLine = cleanLine(line);

        if (cleanedLine.empty())
            continue;

        if (cleanedLine.front() == '{') {
            currentThread = Threads();
            inThreadBlock = true;
            continue;
        }

        if (inThreadBlock && cleanedLine.front() == '}') {
            if (channel.channelUuid == currentThread.channelUuid) {
                getComments(currentThread);
                channel.threads.push_back(currentThread);
            }
            inThreadBlock = false;
            continue;
        }

        if (!inThreadBlock)
            continue;

        std::size_t colonPos = cleanedLine.find(':');

        if (colonPos == std::string::npos)
            continue;

        std::string key = trim(cleanedLine.substr(0, colonPos));
        std::string value = trim(cleanedLine.substr(colonPos + 1));

        if (!value.empty() && value.back() == ',')
            value.pop_back();
        if (!key.empty() && key.front() == '"' && key.back() == '"')
            key = key.substr(1, key.size() - 2);
        if (!value.empty() && value.front() == '"' && value.back() == '"')
            value = value.substr(1, value.size() - 2);
        if (key == "name")
            currentThread.name = value;
        else if (key == "threadUuid")
            currentThread.threadUuid = value;
        else if (key == "channelUuid")
            currentThread.channelUuid = value;
        else if (key == "description")
            currentThread.description = value;
        else if (key == "authorUuid")
            currentThread.authorUuid = value;
    }
    file.close();

    for (const auto& thread : channel.threads) {
        if (thread.name.empty() || thread.threadUuid.empty() || thread.channelUuid.empty() || thread.authorUuid.empty())
            throw std::runtime_error("Invalid thread file format: missing required fields (name, threadUuid, channelUuid and authorUuid)");
    }
}

void Parsing::getComments(Threads& thread)
{
    if (!std::filesystem::exists(_commentFilePath))
        return;

    std::ifstream file(_commentFilePath);

    if (!checkBrackets(file))
        throw std::runtime_error("Invalid comment file format: missing brackets");
    file.close();

    file.open(_commentFilePath);

    bool inCommentBlock = false;
    Comment currentComment;

    for (std::string line; std::getline(file, line);) {
        std::string cleanedLine = cleanLine(line);

        if (cleanedLine.empty())
            continue;

        if (cleanedLine.front() == '{') {
            currentComment = Comment();
            inCommentBlock = true;
            continue;
        }

        if (inCommentBlock && cleanedLine.front() == '}') {
            if (thread.threadUuid == currentComment.threadUuid)
                thread.comments.push_back(currentComment);
            inCommentBlock = false;
            continue;
        }

        if (!inCommentBlock)
            continue;

        std::size_t colonPos = cleanedLine.find(':');

        if (colonPos == std::string::npos)
            continue;

        std::string key = trim(cleanedLine.substr(0, colonPos));
        std::string value = trim(cleanedLine.substr(colonPos + 1));

        if (!value.empty() && value.back() == ',')
            value.pop_back();
        if (!key.empty() && key.front() == '"' && key.back() == '"')
            key = key.substr(1, key.size() - 2);
        if (!value.empty() && value.front() == '"' && value.back() == '"')
            value = value.substr(1, value.size() - 2);
        if (key == "body")
            currentComment.body = value;
        else if (key == "commentUuid")
            currentComment.commentUuid = value;
        else if (key == "threadUuid")
            currentComment.threadUuid = value;
        else if (key == "authorUuid" || key == "userUuid")
            currentComment.authorUuid = value;
    }
    file.close();

    for (const auto& comment : thread.comments) {
        if (comment.body.empty() || comment.commentUuid.empty() || comment.threadUuid.empty() || comment.authorUuid.empty())
            throw std::runtime_error("Invalid comment file format: missing required fields (body, commentUuid, threadUuid and authorUuid)");
    }
}

const std::vector<Parsing::privateMessages>& Parsing::getPrivateMessages()
{
    std::ifstream file(_privateMessageFilePath);

    if (!checkBrackets(file))
        throw std::runtime_error("Invalid private message file format: missing brackets");
    file.close();

    file.open(_privateMessageFilePath);

    bool inMessageBlock = false;
    privateMessages currentMessage;

    for (std::string line; std::getline(file, line);) {
        std::string cleanedLine = cleanLine(line);

        if (cleanedLine.empty())
            continue;

        if (cleanedLine.front() == '{') {
            currentMessage = privateMessages();
            inMessageBlock = true;
            continue;
        }

        if (inMessageBlock && cleanedLine.front() == '}') {
            _privateMessages.push_back(currentMessage);
            inMessageBlock = false;
            continue;
        }

        if (!inMessageBlock)
            continue;

        std::size_t colonPos = cleanedLine.find(':');

        if (colonPos == std::string::npos)
            continue;

        std::string key = trim(cleanedLine.substr(0, colonPos));
        std::string value = trim(cleanedLine.substr(colonPos + 1));

        if (!value.empty() && value.back() == ',')
            value.pop_back();
        if (!key.empty() && key.front() == '"' && key.back() == '"')
            key = key.substr(1, key.size() - 2);
        if (!value.empty() && value.front() == '"' && value.back() == '"')
            value = value.substr(1, value.size() - 2);
        if (key == "senderUuid")
            currentMessage.senderUuid = value;
        else if (key == "receiverUuid")
            currentMessage.receiverUuid = value;
        else if (key == "body")
            currentMessage.body = value;
        else if (key == "timestamp")
            currentMessage.timestamp = std::stoll(value);
    }
    file.close();

    for (const auto& message : _privateMessages) {
        if (message.senderUuid.empty() || message.receiverUuid.empty() || message.body.empty() || message.timestamp == 0)
            throw std::runtime_error("Invalid private message file format: missing required fields (senderUuid, receiverUuid, body and timestamp)");
    }
    return _privateMessages;
}
