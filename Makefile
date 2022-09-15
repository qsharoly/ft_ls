NAME=ft_ls

CC := clang
#CC := /home/debby/Downloads/zig/zig-linux-x86_64-0.8.0/zig cc
#CC := /home/debby/Downloads/zig/zig-linux-x86_64-0.9.0-dev.847+c465b34d3/zig cc

CFLAGS +=-Iincludes -Ilibft/includes -Ilibftprintf/includes
CFLAGS +=-Wall -Wextra -Werror

#each module will add to this
SRC := src/main.c\
	   src/columnize.c\

include libft/module
include libftprintf/module

debug ?= -g
optimize ?= -O2

.phony: all clean fclean re

all:  $(NAME)

$(NAME): $(SRC) includes/*.h libft/includes/*.h libftprintf/includes/*.h
	$(CC) $(debug) $(optimize) $(CFLAGS) -o $@ $(SRC) $(LIBS)

clean:
fclean: clean
	rm -f $(NAME) 
re: fclean
	make all
