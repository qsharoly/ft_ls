#include "../libft/includes/libft.h"
#include "../includes/libftprintf.h"
#include <stdio.h>

int		main(int argc, char **argv)
{
	double	d;
	int		i;
	__attribute__((__format__(printf, 1, 2)))
	int		(*printf_call)(const char *, ...);

	printf_call = ft_printf;
	if (argc > 1 && ft_strequ(argv[1], "-libc"))
		printf_call = printf;
	d = 0.3 * (1 << 20);
	i = 0;
	while (i < 100000)
	{
		printf_call("%f\n", d);
		d *= 2;
		i++;
	}
	return (0);
}
