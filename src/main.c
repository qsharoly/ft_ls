/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: debby <marvin@42.fr>                       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/03/10 07:56:32 by debby             #+#    #+#             */
/*   Updated: 2021/03/10 22:44:04 by debby            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"
#include "libftprintf.h"

#define STARTING_DEPTH 0
#define MAX_DEPTH 25
#define MAX_WIDTH 1000

#define BLOCK_MAGIC 2

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
#include <time.h>

char	*patcat(const char *path, const char *name)
{
	char	*str;

	str = malloc(ft_strlen(path) + 1 + ft_strlen(name) + 1);
	if (!str)
	{
		perror("ft_ls: malloc failed");
		exit(2);
	}
	ft_strcpy(str, path);
	ft_strcat(str, "/");
	ft_strcat(str, name);
	return (str);
}

struct		s_about
{
	struct stat		info;
	char			*name;
	char			*fullpath;
};

int		alpha(const void *l, const void *r)
{
	struct s_about	*left;
	struct s_about	*right;

	left = *(struct s_about **)l;
	right = *(struct s_about **)r;
	return (ft_strcmpi(left->name + (*(left->name) == '.'), right->name + (*(right->name) == '.')));
}

int		rev_alpha(const void *l, const void *r)
{
	struct s_about	*left;
	struct s_about	*right;

	left = *(struct s_about **)l;
	right = *(struct s_about **)r;
	return (-ft_strcmpi(left->name + (*(left->name) == '.'), right->name + (*(right->name) == '.')));
}

int		mtime(const void *l, const void *r)
{
	struct s_about	*left;
	struct s_about	*right;

	left = *(struct s_about **)l;
	right = *(struct s_about **)r;
	return (right->info.st_mtime - left->info.st_mtime);
}

int		rev_mtime(const void *l, const void *r)
{
	struct s_about	*left;
	struct s_about	*right;

	left = *(struct s_about **)l;
	right = *(struct s_about **)r;
	return (-(right->info.st_mtime - left->info.st_mtime));
}

size_t	n_digits(size_t val)
{
	size_t	n;

	n = 1;
	while (val)
	{
		val /= 10;
		n++;
	}
	return (n);
}

void	list_path(const char *path, int depth, int options)
{
	struct stat		info;
	DIR				*dir;
	struct dirent	*entry;
	struct s_about	*about[MAX_WIDTH];
	int				count;
	int				i;
	int				(*sorting)(const void *left, const void *right);
	size_t			tot_blocks;
	size_t			max_lnk;
	size_t			max_size;
	size_t			max_name;
	char			perms[11];

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
		dir = opendir(path);
		count = 0;
		tot_blocks = 0;
		max_lnk = 0;
		max_size = 0;
		max_name = 0;
		while ((entry = readdir(dir)))
		{
			if (entry->d_name[0] == '.' && !(options & LS_LIST_ALL))
				continue;
			if (count > MAX_WIDTH)
			{
				ft_printf("ft_ls: reached max number of entries\n");
				exit(2);
			}
			about[count] = malloc(sizeof(struct s_about));
			if (!about[count])
			{
				perror("ft_ls: malloc failed");
				exit(2);
			}
			about[count]->name = ft_strdup(entry->d_name);
			if (!about[count]->name)
			{
				perror("ft_ls: malloc failed");
				exit(2);
			}
			about[count]->fullpath = patcat(path, entry->d_name);
			if (-1 == stat(about[count]->fullpath, &(about[count]->info)))
			{
				ft_printf("ft_ls: 2 cannot access '%s': %s\n", entry->d_name, strerror(errno));
				exit(2);
			}
			tot_blocks += about[count]->info.st_blocks;
			max_lnk = ft_max(max_lnk, n_digits(about[count]->info.st_nlink));
			max_size = ft_max(max_size, n_digits(about[count]->info.st_size));
			max_name = ft_max(max_name, ft_strlen(about[count]->name));
			count++;
		}
		closedir(dir);
		/*
		 * sort() here
		 */
		sorting = alpha;
		if (options & LS_SORT_TIME)
			sorting = mtime;
		if (options & LS_SORT_REVERSE)
		{
			sorting = sorting == alpha ? rev_alpha : rev_mtime;
		}
		qsort(about, count, sizeof(struct s_about *), sorting);
		/*
		 * end sort()
		 */
		if (options & LS_RECURSE)
		{
			ft_printf("%s:", path);
			if (count > 1)
				ft_printf("\n");
		}
		if (options & LS_LONG_INFO)
			ft_printf("total %lu", tot_blocks / BLOCK_MAGIC);
		i = 0;
		while (i < count)
		{
			if (options & LS_LONG_INFO)
			{
				mode_t	mode = about[i]->info.st_mode;
				perms[0] = mode & S_IFDIR ? 'd' : '-';
				perms[1] = mode & S_IRUSR ? 'r' : '-';
				perms[2] = mode & S_IWUSR ? 'w' : '-';
				perms[3] = mode & S_IXUSR ? 'x' : '-';
				perms[4] = mode & S_IRGRP ? 'r' : '-';
				perms[5] = mode & S_IWGRP ? 'w' : '-';
				perms[6] = mode & S_IXGRP ? 'x' : '-';
				perms[7] = mode & S_IROTH ? 'r' : '-';
				perms[8] = mode & S_IWOTH ? 'w' : '-';
				perms[9] = mode & S_IXOTH ? 'x' : '-';
				perms[10] = '\0';
				size_t nlnk = about[i]->info.st_nlink;
				size_t siz = about[i]->info.st_size;
				char *tim = ctime(&(about[i]->info.st_mtime));
				tim = ft_strchr(tim, ' ') + 1;
				char *timfin = ft_strrchr(tim, ':');
				*timfin = '\0';
				char *nam = about[i]->name;
				ft_printf("\n%s %*lu %*lu %s %-*s", perms, max_lnk - 1, nlnk, max_size - 1, siz, tim, max_name, nam);
			}
			else
				ft_printf("%s\t", about[i]->name);
			i++;
		}
		if (!(options & LS_RECURSE))
			return ;
		i = 0;
		while (i < count)
		{
			if ((about[i]->info.st_mode & S_IFDIR)
				&& !(ft_strequ(about[i]->name, ".") || ft_strequ(about[i]->name, "..")))
			{
				ft_printf("\n\n");
				list_path(about[i]->fullpath, depth + 1, options);
			}
			free(about[i]->name);
			free(about[i]->fullpath);
			free(about[i]);
			i++;
		}
	}
	else
		ft_printf("%s", path);
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
