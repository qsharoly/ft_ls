NAME=ft_ls
LFT=libft
LPF=libftprintf
CC=gcc
CCFLAGS=-Wall -Wextra -Werror
INCFLAGS=-Iincludes -I$(LFT)/includes -I$(LPF)/includes
LIBFLAGS=-L$(LFT) -lft -L$(LPF) -lftprintf
SRC=src/main.c

DEBUG=1
ifdef DEBUG
	CCFLAGS += -g
endif

.phony: all clean fclean re

all: $(NAME) $(LFT) $(LPF)

export DEBUG
export CC
$(NAME): $(SRC) includes/ft_ls.h
	make -C $(LPF)
	$(CC) $(SRC) -o $(NAME) $(CCFLAGS) $(INCFLAGS) $(LIBFLAGS)

clean:
	rm -f *.o
	make -C $(LPF) clean
fclean: clean
	rm -f ft_ls 
	make -C $(LPF) fclean
re: fclean
	make all
