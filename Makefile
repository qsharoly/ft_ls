NAME ?= ft_ls

CC ?= clang

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
