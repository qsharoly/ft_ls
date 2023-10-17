#include "libft.h"
#include "libftprintf.h"
#include <stdio.h>
#include <stdlib.h>

typedef enum e_ok {Ok, Fail} t_ok;
typedef int (*t_printf_call)(const char *, ...);

static t_ok	parse_a_number(const char *s, int *value);
static void parse_arguments(t_printf_call *printf_call, int *n_iterations,
							int argc, char * const *argv);

typedef enum e_ok {Ok, Fail} t_ok;
typedef int (*t_printf_call)(const char *, ...);

static t_ok	parse_a_number(const char *s, int *value);
static void parse_arguments(t_printf_call *printf_call, int *n_iterations,
							int argc, char * const *argv);

int		main(int argc, char **argv)
{
	double	d;
	double	mul;
	int		i;
	int		n_iterations = 1000;
	__attribute__((__format__(printf, 1, 2)))
	t_printf_call printf_call = ft_printf;

	parse_arguments(&printf_call, &n_iterations, argc, argv);
	d = 0.3 * (1 << 20);
	mul = 2.0;
	i = 0;
	while (i < n_iterations)
	{
		printf_call("%5d: ", i + 1);
		printf_call("%f\n", d);
		if (d > __DBL_MAX__ / 2.0)
			mul = 0.5;
		if (d < __DBL_MIN__ * 2.0)
			mul = 2.0;
		d *= mul;
		i++;
	}
	return (0);
}

static t_ok	parse_a_number(const char *s, int *value)
{
	*value = 0;
	while (*s >= '0' && *s <= '9')
	{
		*value = *value * 10 + (*s - '0');
		s++;
	}
	if (*s == '\0')
		return Ok;
	else
		return Fail;
}

static void parse_arguments(t_printf_call *printf_call, int *n_iterations,
							int argc, char * const *argv)
{
	argv++;
	argc--;
	while (argc > 0)
	{
		if (ft_strequ(*argv, "-libc"))
		{
			argv++;
			argc--;
			*printf_call = printf;
			continue;
		}
		else if (ft_strequ(*argv, "-iter"))
		{
			argv++;
			argc--;
			if (argc < 1)
			{
				printf(" -iter: must pass a number\n");
				exit(1);
			}
			else if (parse_a_number(*argv, n_iterations) == Ok)
			{
				argv++;
				argc--;
				continue;
			}
			else
			{
				printf(" -iter: cannot parse number '%s'\n", *argv);
				exit(1);
			}
		}
		else
		{
			printf(" bad argument: '%s'\n", *argv);
			exit(1);
		}
	}
}
