##
## EPITECH PROJECT, 2024
## myftp
## File description:
## Makefile
##

SRC		=	src/main.c								\
			src/server.c							\
			src/clients_handling.c					\
			src/utils.c								\
			src/execute_commands.c					\
			src/commands/user.c						\
			src/commands/quit.c						\
			src/commands/pass.c						\
			src/commands/help.c						\
			src/commands/pwd.c						\
			src/commands/pasv.c						\
			src/commands/port.c						\
			src/commands/cwd.c						\
			src/commands/cdup.c						\
			src/commands/noop.c						\
			src/commands/dele.c						\
			src/commands/list/list.c				\
			src/commands/list/list_active.c			\
			src/commands/list/list_passive.c		\
			src/commands/retr/retr.c				\
			src/commands/retr/retr_passive.c		\
			src/commands/retr/retr_active.c			\
			src/commands/stor/stor.c				\
			src/commands/stor/stor_active.c			\
			src/commands/stor/stor_passive.c		\

	



NAME	=	myftp
OBJ		=	$(SRC:.c=.o)
CC		=	epiclang
CFLAGS	=	-Iinclude -Wall -Wextra -g

all: $(NAME)

$(NAME): $(OBJ)
	@$(CC) $(CFLAGS) -o $(NAME) $(OBJ)
	@echo "Compilation done"

clean:
	@rm -f $(OBJ)
	@echo "Cleaning done"

fclean: clean
	@rm -f $(NAME)
	@echo "Full cleaning done"

re: fclean all