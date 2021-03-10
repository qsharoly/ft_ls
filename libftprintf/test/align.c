#include "libftprintf.h"
#include <stdalign.h>

int main(void)
{
	ft_printf("alignof:\n");
	ft_printf("t_fmt: %lu\n", alignof(t_fmt));
	ft_printf("t_stream: %lu\n", alignof(t_stream));
	ft_printf("t_parts: %lu\n", alignof(t_parts));
	ft_printf("enum e_size: %lu\n", alignof(enum e_size));
	return 0;
}
