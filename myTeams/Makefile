##
## EPITECH PROJECT, 2026
## myteams
## File description:
## Makefile
##

.PHONY: all clean fclean re

all:
	@echo "Building client and server..."
	@$(MAKE) -C client >/dev/null
	@$(MAKE) -C server >/dev/null
	@echo "Build complete."

clean:
	@echo "Cleaning client and server..."
	@$(MAKE) -C client clean >/dev/null
	@$(MAKE) -C server clean >/dev/null
	@echo "Clean complete."

fclean:
	@echo "Fully cleaning client and server..."
	@$(MAKE) -C client fclean >/dev/null
	@$(MAKE) -C server fclean >/dev/null
	@echo "Fclean complete."

re: fclean all
