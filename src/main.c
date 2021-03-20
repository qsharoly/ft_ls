/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: debby <marvin@42.fr>                       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/03/10 07:56:32 by debby             #+#    #+#             */
/*   Updated: 2021/03/20 10:54:09 by debby            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"
#include "libftprintf.h"

#define STARTING_DEPTH 0
#define MAX_DEPTH 25
#define MAX_WIDTH 1000

#define BLOCK_HACK 2

#define HAS_FILENAMES 1
#define LS_LIST_ALL (1<<1)
#define LS_LONG_INFO (1<<2)
#define LS_SORT_REVERSE (1<<3)
#define LS_RECURSE (1<<4)
#define LS_SORT_BY_TIME (1<<5)
#define LS_LIST_BY_LINES_INSTEAD_OF_COLUMNS (1<<6)

enum e_exitcode
{
	Success = 0,
	Fail_minor = 1,
	Fail_serious = 2,
};

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
		"  -x\t\tlist by lines instead of columns\n"
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
				exit(Success);
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
					options |= LS_SORT_BY_TIME;
				else if (c == 'x')
					options |= LS_LIST_BY_LINES_INSTEAD_OF_COLUMNS;
				else
				{
					ft_printf("ft_ls: invalid option -- '%c'\n", c);
					ft_printf("Try 'ft_ls --help' for more information.\n");
					exit(Fail_serious);
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
#include <sys/ioctl.h>
#include <dirent.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>
#include <time.h>
#include <pwd.h>
#include <grp.h>

char	*patcat(const char *path, const char *name)
{
	char	*str;

	str = malloc(ft_strlen(path) + 1 + ft_strlen(name) + 1);
	if (!str)
	{
		perror("ft_ls: malloc failed");
		exit(Fail_serious);
	}
	ft_strcpy(str, path);
	ft_strcat(str, "/");
	ft_strcat(str, name);
	return (str);
}

struct		s_finfo
{
	struct stat		info;
	char			*name;
	char			*fullpath;
	char			*owner;
	char			*group;
};

int		alpha(const void *l, const void *r)
{
	struct s_finfo	*left;
	struct s_finfo	*right;

	left = *(struct s_finfo **)l;
	right = *(struct s_finfo **)r;
	return (ft_strcmpi(left->name + (*(left->name) == '.'), right->name + (*(right->name) == '.')));
}

int		rev_alpha(const void *l, const void *r)
{
	struct s_finfo	*left;
	struct s_finfo	*right;

	left = *(struct s_finfo **)l;
	right = *(struct s_finfo **)r;
	return (-ft_strcmpi(left->name + (*(left->name) == '.'), right->name + (*(right->name) == '.')));
}

int		mtime(const void *l, const void *r)
{
	struct s_finfo	*left;
	struct s_finfo	*right;

	left = *(struct s_finfo **)l;
	right = *(struct s_finfo **)r;
	return (right->info.st_mtime - left->info.st_mtime);
}

int		rev_mtime(const void *l, const void *r)
{
	struct s_finfo	*left;
	struct s_finfo	*right;

	left = *(struct s_finfo **)l;
	right = *(struct s_finfo **)r;
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

void	my_assert(int condition, const char *message, enum e_exitcode code)
{
	if (!condition)
	{
		perror(message);
		exit(code);
	}
}

void	list_path(const char *path, int depth, int options)
{
	struct stat		info;
	DIR				*dir;
	struct dirent	*entry;
	struct s_finfo	*about[MAX_WIDTH];
	int				count;
	int				i;
	int				(*sorting)(const void *left, const void *right);
	size_t			tot_blocks;
	size_t			max_lnk;
	size_t			max_size;
	size_t			max_name;
	size_t			max_owner;
	size_t			max_group;
	char			perms[11];

	if (depth > MAX_DEPTH)
	{
		ft_printf("%s: reached recursion limit. exiting.\n", path);
		exit(Fail_minor);
	}
	if (-1 == stat(path, &info))
	{
		ft_printf("ft_ls: cannot access '%s': %s\n", path, strerror(errno));
		if (depth > STARTING_DEPTH)
			exit(Fail_minor);
		else
			exit(Fail_serious);
	}
	if (info.st_mode & S_IFDIR)
	{
		dir = opendir(path);
		count = 0;
		tot_blocks = 0;
		max_lnk = 0;
		max_size = 0;
		max_name = 0;
		max_owner = 0;
		max_group = 0;
		while ((entry = readdir(dir)))
		{
			if (entry->d_name[0] == '.' && !(options & LS_LIST_ALL))
				continue;
			if (count > MAX_WIDTH)
			{
				ft_printf("%s: reached max number of entries. exiting.\n", path);
				exit(Fail_serious);
			}
			about[count] = malloc(sizeof(struct s_finfo));
			my_assert(!!about[count], "ft_ls: malloc failed", Fail_serious);
			about[count]->name = ft_strdup(entry->d_name);
			my_assert(!!about[count]->name, "ft_ls: malloc failed", Fail_serious);
			about[count]->fullpath = patcat(path, entry->d_name);
			int				res;
			struct passwd	*passwd_data;
			struct group	*group_data;
			res = stat(about[count]->fullpath, &(about[count]->info));
			if (-1 == res)
			{
				ft_printf("ft_ls: cannot access '%s': %s\n", entry->d_name, strerror(errno));
				exit(Fail_serious);
			}
			passwd_data = getpwuid(about[count]->info.st_uid);
			my_assert(!!passwd_data, "ft_ls: unable to get owner's name", Fail_serious);
			group_data = getgrgid(about[count]->info.st_gid);
			my_assert(!!group_data, "ft_ls: unable to get groupname", Fail_serious);
			about[count]->owner = ft_strdup(passwd_data->pw_name);
			my_assert(!!about[count]->owner, "ft_ls: malloc failed", Fail_serious);
			about[count]->group = ft_strdup(group_data->gr_name);
			my_assert(!!about[count]->group, "ft_ls: malloc failed", Fail_serious);
			tot_blocks += about[count]->info.st_blocks;
			max_lnk = ft_max(max_lnk, n_digits(about[count]->info.st_nlink));
			max_size = ft_max(max_size, n_digits(about[count]->info.st_size));
			max_name = ft_max(max_name, ft_strlen(about[count]->name));
			max_owner = ft_max(max_owner, ft_strlen(about[count]->owner));
			max_group = ft_max(max_group, ft_strlen(about[count]->group));
			count++;
		}
		closedir(dir);
		/*
		 * sort() here
		 */
		sorting = alpha;
		if (options & LS_SORT_BY_TIME)
			sorting = mtime;
		if (options & LS_SORT_REVERSE)
		{
			sorting = sorting == alpha ? rev_alpha : rev_mtime;
		}
		qsort(about, count, sizeof(struct s_finfo *), sorting);
		/*
		 * end sort()
		 */
		if (options & LS_RECURSE)
		{
			ft_printf("%s:\n", path);
		}
		struct winsize	winsize;
		int				ncol;
		if (options & LS_LONG_INFO)
		{
			ft_printf("total %lu\n", tot_blocks / BLOCK_HACK);
		}
		else
		{
			/*
			 * TODO(qsharoly): match output column widths of ls
			 */
			int	res;

			res = ioctl(0, TIOCGWINSZ, &winsize);
			my_assert(res != -1, "ft_ls: failed to get terminal dimensions", Fail_serious);
			ncol = max_name > 0 ? winsize.ws_col / max_name : 1;
		}
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
				size_t nlink = about[i]->info.st_nlink;
				size_t fsize = about[i]->info.st_size;
				char *tim = ctime(&(about[i]->info.st_mtime));
				tim = ft_strchr(tim, ' ') + 1;
				char *timfin = ft_strrchr(tim, ':');
				*timfin = '\0';
				char *nam = about[i]->name;
				ft_printf("%s %*lu %-*s %-*s %*lu %s %-s\n", perms, max_lnk - 1, nlink, max_owner - 1, about[i]->owner, max_group - 1, about[i]->group, max_size - 1, fsize, tim, nam);
			}
			else
			{
				if (options & LS_LIST_BY_LINES_INSTEAD_OF_COLUMNS)
				{

					ft_printf("%-*s", max_name, about[i]->name);
				}
				else
				{
					int	tabindex;

					tabindex = i / ncol + (count / ncol + 1) * (i % ncol);
					if (tabindex < count)
						ft_printf("%-*s", max_name, about[tabindex]->name);
				}
				if (0 == (i + 1) % ncol || i == count - 1)
					ft_printf("\n");
			}
			i++;
		}
		i = 0;
		while (i < count)
		{
			if ((options & LS_RECURSE) && (about[i]->info.st_mode & S_IFDIR)
				&& !(ft_strequ(about[i]->name, ".") || ft_strequ(about[i]->name, "..")))
			{
				ft_printf("\n");
				list_path(about[i]->fullpath, depth + 1, options);
			}
			free(about[i]->name);
			free(about[i]->fullpath);
			free(about[i]);
			i++;
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
	return (0);
}
