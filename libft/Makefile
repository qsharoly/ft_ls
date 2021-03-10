NAME = libft.a
SONAME = libft.so
CC = gcc
CCFLAGS = -Wall -Wextra -Werror
SRCDIR = .
INCDIR = includes
OBJDIR = objects
DEPDIR = deps
DEPFLAGS = -MT $@ -MMD -MP -MF $(DEPDIR)/$*.d

ifdef DEBUG
	CCFLAGS += -g
endif

SRC = $(shell ls ft_*.c)
OBJ = $(SRC:%.c=$(OBJDIR)/%.o)
DEPS = $(SRC:%.c=$(DEPDIR)/%.d)

$(shell mkdir -p $(OBJDIR))
$(shell mkdir -p $(DEPDIR))

.PHONY: all clean fclean re
all: $(NAME)

$(NAME): $(OBJ)
	@echo "# packing $(NAME)"
	ar rcs $(NAME) $(OBJ)

so: $(OBJ)
	gcc --shared $(OBJ) -o $(SONAME)

$(OBJDIR)/%.o: $(SRCDIR)/%.c
$(OBJDIR)/%.o: $(SRCDIR)/%.c $(DEPDIR)/%.d
##	$(CC) -c $< -o $@ -I $(INCDIR) $(CCFLAGS) $(DEPFLAGS)
	$(CC) -O3 -fpic -c $< -o $@ -I $(INCDIR) $(CCFLAGS) $(DEPFLAGS)

$(DEPDIR)/%d: ;

clean:
	rm -rf $(OBJDIR) $(DEPDIR)
fclean: clean
	rm -f $(NAME) $(SONAME)
re: fclean
	make all

-include $(DEPS)
