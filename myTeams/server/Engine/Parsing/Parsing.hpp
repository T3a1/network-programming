/*
** EPITECH PROJECT, 2026
** G-NWP-400-LIL-4-1-myteams-2
** File description:
** Parsing
*/

#ifndef Parsing_HPP_
#define Parsing_HPP_

#include <fstream>
#include <string>
#include <vector>
#include <fcntl.h>
#include <fstream>
#include <stdexcept>
#include <iostream>

class Parsing {

    
    public:
    Parsing(std::string userFilePath, std::string teamFilePath, std::string channelFilePath, std::string threadFilePath, std::string commentFilePath, std::string privateMessageFilePath);
    ~Parsing();

    struct User {
        std::string name;
        std::string userUuid;   
    };

    struct privateMessages
    {
        std::string senderUuid;
        std::string receiverUuid;
        std::string body;
        time_t timestamp;
    };
    

    struct Comment
    {
        std::string body;
        std::string commentUuid;
        std::string threadUuid;
        std::string authorUuid;
    };
    
    struct Threads
    {
        std::string name;
        std::string threadUuid;
        std::string channelUuid;
        std::string description;
        std::string authorUuid;
        std::vector<User> authorizedUsers;
        std::vector<Comment> comments;
    };

    struct Channel
    {
        std::string name;
        std::string channelUuid;
        std::string teamUuid;
        std::string description;
        std::vector<User> authorizedUsers;
        std::vector<Threads> threads;
    };

    struct Team
    {
        std::string name;
        std::string teamUuid;
        std::string description;
        std::vector<User> authorizedUsers;
        std::vector<Channel> channels;
    };

    
    

    const std::vector<User>& getUsers();
    const std::vector<privateMessages>& getPrivateMessages();
    const std::vector<Team>& getTeams();
    void getChannels(Team& team);
    void getThreads(Channel& channel);
    void getComments(Threads& thread);
    bool checkBrackets(std::ifstream& file) const;
    std::string cleanLine(const std::string& line) const;
    std::string trim(const std::string& line) const;

    protected:
    private:
        std::vector<User> _users;
        std::vector<Team> _teams;
        std::vector<Channel> _channels;
        std::vector<Threads> _threads;
        std::vector<Comment> _comments;
        std::vector<privateMessages> _privateMessages;
        std::string _userFilePath;
        std::string _teamFilePath;
        std::string _channelFilePath;
        std::string _threadFilePath;
        std::string _commentFilePath;
        std::string _privateMessageFilePath;
};

#endif /* !Parsing_HPP_ */
