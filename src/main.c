/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: debby <marvin@42.fr>                       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/03/10 07:56:32 by debby             #+#    #+#             */
/*   Updated: 2021/03/10 17:56:56 by debby            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"
#include "libftprintf.h"

#define STARTING_DEPTH 0
#define MAX_DEPTH 25
#define MAX_WIDTH 1000

#define LS_LIST_ALL 1
#define LS_LONG_INFO 2
#define LS_SORT_REVERSE 4
#define LS_RECURSE 8
#define LS_SORT_TIME 16
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

static unsigned	parse_all_options(int argc, const char **argv)
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
					options |= LS_LIST_ALL;
				else if (c == 'l')
					options |= LS_LONG_INFO;
				else if (c == 'r')
					options |= LS_SORT_REVERSE;
				else if (c == 'R')
					options |= LS_RECURSE;
				else if (c == 't')
					options |= LS_SORT_TIME;
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

#include <sys/stat.h>
#include <dirent.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>

char	*patcat(const char *path, const char *name)
{
	char	*str;

	str = malloc(ft_strlen(path) + 1 + ft_strlen(name) + 1);
	if (!str)
	{
		perror("ft_ls 3");
		exit(2);
	}
	ft_strcpy(str, path);
	ft_strcat(str, "/");
	ft_strcat(str, name);
	return (str);
}

void	list_path(const char *path, int depth, int options)
{
	struct stat		info;
	DIR				*dir;
	struct dirent	*entry;
	char			*subfile[MAX_WIDTH];
	int				is_subdir[MAX_WIDTH];
	int				count;
	int				i;

	if (depth > MAX_DEPTH)
	{
		ft_printf("ft_ls: reached recursion limit\n");
		exit(1);
	}
	if (-1 == stat(path, &info))
	{
		ft_printf("ft_ls: 1 cannot access '%s': %s\n", path, strerror(errno));
		if (depth > STARTING_DEPTH)
			exit(1);
		else
			exit(2);
	}
	if (info.st_mode & S_IFDIR)
	{
		if (options & LS_RECURSE)
			ft_printf("%s:\n", path);
		dir = opendir(path);
		count = 0;
		while ((entry = readdir(dir)))
		{
			if (entry->d_name[0] == '.' && !(options & LS_LIST_ALL))
				continue;
			ft_printf("%s\t", entry->d_name);
			if (options & LS_RECURSE)
			{
				if (ft_strequ(entry->d_name,  ".") || ft_strequ(entry->d_name, ".."))
					continue;
				if (count > MAX_WIDTH)
				{
					ft_printf("ft_ls: reached max number of entries\n");
					exit(2);
				}
				subfile[count] = patcat(path, entry->d_name);
				if (-1 == stat(subfile[count], &info))
				{
					ft_printf("ft_ls: 2 cannot access '%s': %s\n", entry->d_name, strerror(errno));
					exit(2);
				}
				is_subdir[count] = !!(info.st_mode & S_IFDIR);
				count++;
			}
		}
		closedir(dir);
		if (options & LS_RECURSE)
		{
			i = 0;
			while (i < count)
			{
				if (is_subdir[i])
				{
					ft_printf("\n\n");
					list_path(subfile[i], depth + 1, options);
				}
				free(subfile[i]);
				i++;
			}
		}
	}
	else
		ft_printf("%s\n", path);
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
			list_path(argv[i], STARTING_DEPTH, options);
			i++;
		}
	}
	else
	{
		list_path(".", STARTING_DEPTH, options);
	}
	ft_printf("\n");
	return (0);
}
