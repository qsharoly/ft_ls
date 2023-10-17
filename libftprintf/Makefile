NAME = libftprintf.a
INCDIR = includes 
OBJDIR = obj
SRCDIR = src
LFT_DIR ?= ../libft
LFT = $(LFT_DIR)/libft.a
CC ?= gcc
CCFLAGS += -Wall -Wextra -Werror
INCFLAGS = -I$(INCDIR) -I$(LFT_DIR)/includes

#Disable union ABI warning on linux gcc
UNAME_S = $(shell uname -s)
ifeq ($(UNAME_S), Linux)
ifeq ($(CC), gcc)
	CCFLAGS += -Wno-psabi
endif
endif

debug ?= -g
optimize ?= -O2

SRC = ft_printf.c\
	  ft_snprintf.c\
	  sv.c\
	  pf_stream.c\
	  pf_stream_helpers.c\
	  pf_parse.c\
	  pf_conv_basic.c\
	  pf_conv_putnbr.c\
	  pf_conv_diuox.c\
	  pf_conv_floating.c\
	  pf_utoa_base.c\
	  pf_dtoa.c\
	  pf_dtoa_quick.c\
	  bignum/big_init.c\
	  bignum/big_mul.c\
	  bignum/big_raise.c\
	  bignum/big_to_str.c\
	  float/fp_fabs.c\
	  float/fp_isneg.c\
	  float/fp_isinf.c\
	  float/fp_isnan.c\
	  float/fp_issub.c\
	  float/fp_trunc.c

OBJ = $(SRC:%.c=$(OBJDIR)/%.o)

$(shell mkdir -p $(OBJDIR) $(OBJDIR)/bignum $(OBJDIR)/float)

export debug
export CC
export optimize

all: $(NAME)

$(LFT):
	make -C $(LFT_DIR)

$(NAME): $(OBJ) $(LFT)
	@echo "# packing $(NAME)"
	cp $(LFT) ./$(NAME)
	ar rcs $(NAME) $(OBJ)

$(OBJDIR)/%.o: $(SRCDIR)/%.c includes/libftprintf.h includes/sv.h
	$(CC) $(optimize) $(debug) -c $< -o $@ $(INCFLAGS) $(CCFLAGS)
$(OBJDIR)/pf_conv_floating.o: includes/float.h
$(OBJDIR)/pf_dtoa_quick.o: includes/float.h
$(OBJDIR)/pf_dtoa.o: includes/bignum.h includes/float.h
$(OBJDIR)/bignum/*.o: includes/bignum.h
$(OBJDIR)/float/*.o: includes/float.h

.PHONY: all clean fclean re

clean:
	rm -rf $(OBJDIR)
	make -C $(LFT_DIR) clean
fclean: clean
	rm -f $(NAME)
	make -C $(LFT_DIR) fclean
re: fclean
	make all
