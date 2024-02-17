BIN ?= ft_ls

CC ?= clang
CCFLAGS ?= -O2 -g
IFLAGS +=-Iincludes -Ilibft/includes -Ilibftprintf/includes
WFLAGS +=-Wall -Wextra -Werror

#each module will add to this
SRC := src/main.c\
	   src/columnize.c\

include libft/module
include libftprintf/module

.phony: all clean fclean re

all:  $(BIN)

$(BIN): $(SRC) src/*.h libft/includes/*.h libftprintf/includes/*.h
	$(CC) $(CCFLAGS) -o $@ $(SRC) $(IFLAGS) $(WFLAGS)

clean:
fclean: clean
	rm -f $(BIN)
re: fclean
	make all
