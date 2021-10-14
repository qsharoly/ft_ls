#include "../../libft/includes/libft.h"
#include "../includes/libftprintf.h"
#include <stdio.h>
#include <limits.h>

int		main(int argc, char **argv)
{
	double	d;
	int		i;
	__attribute__((__format__(printf, 1, 2)))
	int		(*printf_call)(const char *, ...);

	printf_call = ft_printf;
	if (argc > 1 && ft_strequ(argv[1], "-libc"))
		printf_call = printf;
	d = 3.3;
	i = 0;
	while (i < 100000)
	{
		printf_call("%f\n", d);
		if (d < (double)ULONG_MAX)
			d *= 1.1;
		i++;
	}
	return (0);
}
