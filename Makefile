BINARY ?= ft_ls

CC ?= clang
debug ?= -g
optimize ?= -O2

CFLAGS += $(debug) $(optimize)
CFLAGS +=-Iincludes -Ilibft/includes -Ilibftprintf/includes
CFLAGS +=-Wall -Wextra -Werror

#each module will add to this
SRC := src/main.c\
	   src/columnize.c\

include libft/module
include libftprintf/module

.phony: all clean fclean re

all:  $(BINARY)

$(BINARY): $(SRC) src/*.h libft/includes/*.h libftprintf/includes/*.h
	$(CC) $(CFLAGS) -o $@ $(SRC) $(LIBS)

clean:
fclean: clean
	rm -f $(BINARY) 
re: fclean
	make all
