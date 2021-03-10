/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: debby <marvin@42.fr>                       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/03/10 07:56:32 by debby             #+#    #+#             */
/*   Updated: 2021/03/10 10:41:07 by debby            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"
#include "libftprintf.h"

#define LONG_LISTING 1
#define REVERSE_SORT 2
#define RECURSIVE 4
#define ALL_INCLUSIVE 8
#define SORT_BY_TIME 16
#define HAS_FILENAMES 32

static void	print_help()
{
	ft_printf(
		"Usage: ft_ls [OPTION]... [FILE]...\n"
		"List information about the FILEs (the current directory by default).\n"
		"Sort entries alphabetically unless -t is specified\n"
		"\n"
		"Options:\n"
		"  -a\t\tdo not ignore entries starting with .\n"
		"  -l\t\tuse a long listing format\n"
		"  -r\t\treverse order while sorting\n"
		"  -R\t\tlist subdirectories recursively\n"
		"  -t\t\tsort by modification time, newest first\n"
		"      --help\tdisplay this help and exit\n"
		"\n"
		"Exit status:\n"
		" 0  if OK,\n"
		" 1  if minor problems (e.g., cannot access subdirectory),\n"
		" 2  if serious trouble (e.g., cannot access command-line argument).\n"
		"\n"
		"Hello world.\n"
	);
}

unsigned	parse_all_options(int argc, const char **argv)
{
	char		c;
	unsigned	options;
	int			i;
	int			j;

	options = 0;
	i = 1;
	while (i < argc)
	{
		if (argv[i][0] == '-')
		{
			if (ft_strequ(argv[i], "--help"))
			{
				print_help();
				exit(0);
			}
			j = 1;
			while ((c = argv[i][j]))
			{
				if (c == 'a')
					options |= ALL_INCLUSIVE;
				else if (c == 'l')
					options |= LONG_LISTING;
				else if (c == 'r')
					options |= REVERSE_SORT;
				else if (c == 'R')
					options |= RECURSIVE;
				else if (c == 't')
					options |= SORT_BY_TIME;
				else
				{
					ft_printf("ft_ls: invalid option -- '%c'\n", c);
					ft_printf("Try 'ft_ls --help' for more information.\n");
					exit(2);
				}
				j++;
			}
		}
		else
			options |= HAS_FILENAMES;
		i++;
	}
	return (options);
}

int		main(int argc, const char **argv)
{
	unsigned	options;
	int			i;

	options = parse_all_options(argc, argv);
	if (argc > 1 && (options & HAS_FILENAMES))
	{
		i = 1;
		while (i < argc)
		{
			if (argv[i][0] == '-')
			{
				i++;
				continue;
			}
			//list_path(argv[i], options);
			ft_printf("%s %u\n", argv[i], options);
			i++;
		}
	}
	else
	{
		//list_path(".", options);
		ft_printf(". %u\n", options);
	}
	return (0);
}
