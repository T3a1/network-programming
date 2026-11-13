##
## EPITECH PROJECT, 2026
## myteams
## File description:
## server Makefile
##

CC = g++
INC =	-IServer -IPollFD -IServerError \
		-IComm -IComm/Team -IComm/Channel -IComm/Thread -IComm/Comment \
		-ISocket -IEngine -IUser -IEngine/Parsing
CXXFLAGS = -I../libs/myteams $(INC)  -Wall -Wextra -std=c++17
LDFLAGS = -L../libs/myteams -lmyteams -luuid
NAME = myteams_server
OBJ = $(SRC:.cpp=.o)

SRC =		main.cpp						\
			Server/Server.cpp				\
			PollFD/PollFD.cpp				\
			Socket/Socket.cpp				\
			Engine/Engine.cpp				\
			Engine/Engine_Commands.cpp		\
			Engine/Engine_Operations.cpp	\
			Engine/Engine_Helpers.cpp		\
			Engine/Parsing/Parsing.cpp	\



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
