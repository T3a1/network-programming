##
## EPITECH PROJECT, 2026
## myteams
## File description:
## client Makefile
##

CC = g++
CXXFLAGS = -I../libs/myteams -I. -IClientEvents -IClientEvents/Notifications -IClientEvents/Commands -IClient -Wall -Wextra -std=c++17
LDFLAGS = -L../libs/myteams -lmyteams
NAME = myteams_cli

SRC = 	main.cpp \
		Client/Client.cpp \
		ClientEvents/Notifications/Receiver.cpp \
		ClientEvents/Notifications/PrivateMessage.cpp \
		ClientEvents/Notifications/ThreadReply.cpp \
		ClientEvents/Notifications/TeamCreated.cpp \
		ClientEvents/Notifications/ChannelCreated.cpp \
		ClientEvents/Notifications/ThreadCreated.cpp \
		ClientEvents/Notifications/UnknownTeam.cpp \
		ClientEvents/Notifications/UnknownChannel.cpp \
		ClientEvents/Notifications/UnknownThread.cpp \
		ClientEvents/Notifications/UnknownUser.cpp \
		ClientEvents/Notifications/UnauthorizedCommand.cpp \
		ClientEvents/Notifications/AlreadyExists.cpp \
		ClientEvents/Commands/LoginCommand.cpp \
		ClientEvents/Commands/LogoutCommand.cpp \
		ClientEvents/Commands/HelpCommand.cpp \
		ClientEvents/Commands/SendCommand.cpp \
		ClientEvents/Commands/MessagesCommand.cpp \
		ClientEvents/Commands/SubscribeCommand.cpp \
		ClientEvents/Commands/SubscribedCommand.cpp \
		ClientEvents/Commands/UnsubscribeCommand.cpp \
		ClientEvents/Commands/UserCommand.cpp \
		ClientEvents/Commands/UsersCommand.cpp \
		ClientEvents/Commands/UseCommand.cpp \
		ClientEvents/Commands/CreateCommand.cpp \
		ClientEvents/Commands/ListCommand.cpp \
		ClientEvents/Commands/InfoCommand.cpp \
		ClientEvents/Commands/CleanCommand.cpp \

OBJ = $(SRC:.cpp=.o)

all: $(NAME)

$(NAME): $(OBJ)
	$(CC) $(CXXFLAGS) $^ -o $@ $(LDFLAGS)
	@mv $(NAME) ..

%.o: %.cpp
	$(CC) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ)

fclean: clean
	rm -f ../$(NAME)

re: fclean all
