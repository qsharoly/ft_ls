/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: debby <marvin@42.fr>                       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/03/10 07:56:32 by debby             #+#    #+#             */
/*   Updated: 2021/04/03 04:08:09 by debby            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_ls.h"
#include "libft.h"
#include "libftprintf.h"
#include <stdlib.h> //malloc(), free(), exit()
#include <sys/stat.h> //struct stat, stat()
#include <sys/ioctl.h> //struct winsize, ioctl()
#include <dirent.h> //readdir(), opendir(), closedir()
#include <string.h> //strerror()
#include <errno.h> //strerror()
#include <stdio.h> //perror()
#include <time.h> //ctime()
#include <pwd.h> //getpwuid()
#include <grp.h> //getgrgid()

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
					options |= LS_VERBOSE;
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
	return (right->status.st_mtime - left->status.st_mtime);
}

int		rev_mtime(const void *l, const void *r)
{
	struct s_finfo	*left;
	struct s_finfo	*right;

	left = *(struct s_finfo **)l;
	right = *(struct s_finfo **)r;
	return (-(right->status.st_mtime - left->status.st_mtime));
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

static void	print_verbose_info(struct s_finfo	*f, struct s_col_widths cols)
{
	mode_t	mode;
	char	perms[11];

	mode = f->status.st_mode;
	perms[0] = '?';
	if (S_ISREG(mode))
		perms[0] = '-';
	else if (S_ISBLK(mode))
		perms[0] = 'b';
	else if (S_ISCHR(mode))
		perms[0] = 'c';
	else if (S_ISDIR(mode))
		perms[0] = 'd';
	else if (S_ISLNK(mode))
		perms[0] = 'l';
	else if (S_ISFIFO(mode))
		perms[0] = 'p';
	else if (S_ISSOCK(mode))
		perms[0] = 's';
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
	size_t nlink = f->status.st_nlink;
	size_t fsize = f->status.st_size;
	char *tim = ctime(&(f->status.st_mtime));
	tim = ft_strchr(tim, ' ') + 1;
	char *timfin = ft_strrchr(tim, ':');
	*timfin = '\0';
	char *nam = f->name;
	ft_printf("%s %*lu %-*s %-*s %*lu %s %-s", perms, cols.lnk - 1, nlink, cols.owner - 1, f->owner, cols.group - 1, f->group, cols.size - 1, fsize, tim, nam);
	if (S_ISLNK(mode))
		ft_printf(" -> %s\n", "linked file");
	else
		ft_printf("\n");
}

void	list_path(const char *path, int depth, int options)
{
	struct stat			tmpstat;
	DIR					*dir;
	struct dirent		*entry;
	struct s_finfo		*infos[MAX_WIDTH];
	int					count;
	int					i;
	int					(*sorting)(const void *left, const void *right);
	int					retcode;
	size_t				tot_blocks;
	struct s_col_widths	cols;

	if (depth > MAX_DEPTH)
	{
		ft_printf("%s: reached recursion limit. exiting.\n", path);
		exit(Fail_minor);
	}
	if (options & LS_VERBOSE)
		retcode = lstat(path, &tmpstat);
	else
		retcode = stat(path, &tmpstat);
	if (-1 == retcode)
	{
		ft_printf("ft_ls: cannot access '%s': %s\n", path, strerror(errno));
		if (depth > STARTING_DEPTH)
			exit(Fail_minor);
		else
			exit(Fail_serious);
	}
	if (tmpstat.st_mode & S_IFDIR)
	{
		dir = opendir(path);
		count = 0;
		tot_blocks = 0;
		cols.lnk = 0;
		cols.size = 0;
		cols.name = 0;
		cols.owner = 0;
		cols.group = 0;
		while ((entry = readdir(dir)))
		{
			if (entry->d_name[0] == '.' && !(options & LS_LIST_ALL))
				continue;
			if (count > MAX_WIDTH)
			{
				ft_printf("%s: reached max number of entries. exiting.\n", path);
				exit(Fail_serious);
			}
			infos[count] = malloc(sizeof(struct s_finfo));
			my_assert(!!infos[count], "ft_ls: malloc failed", Fail_serious);
			infos[count]->name = ft_strdup(entry->d_name);
			my_assert(!!infos[count]->name, "ft_ls: malloc failed", Fail_serious);
			infos[count]->fullpath = patcat(path, entry->d_name);
			retcode = lstat(infos[count]->fullpath, &(infos[count]->status));
			if (-1 == retcode)
			{
				ft_printf("ft_ls: cannot access '%s': %s\n", entry->d_name, strerror(errno));
				exit(Fail_serious);
			}
			struct passwd	*passwd_data;
			struct group	*group_data;
			passwd_data = getpwuid(infos[count]->status.st_uid);
			my_assert(!!passwd_data, "ft_ls: unable to get owner's name", Fail_serious);
			infos[count]->owner = ft_strdup(passwd_data->pw_name);
			my_assert(!!infos[count]->owner, "ft_ls: malloc failed", Fail_serious);
			group_data = getgrgid(infos[count]->status.st_gid);
			my_assert(!!group_data, "ft_ls: unable to get groupname", Fail_serious);
			infos[count]->group = ft_strdup(group_data->gr_name);
			my_assert(!!infos[count]->group, "ft_ls: malloc failed", Fail_serious);
			tot_blocks += infos[count]->status.st_blocks;
			cols.lnk = ft_max(cols.lnk, n_digits(infos[count]->status.st_nlink));
			cols.size = ft_max(cols.size, n_digits(infos[count]->status.st_size));
			cols.name = ft_max(cols.name, ft_strlen(infos[count]->name));
			cols.owner = ft_max(cols.owner, ft_strlen(infos[count]->owner));
			cols.group = ft_max(cols.group, ft_strlen(infos[count]->group));
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
		qsort(infos, count, sizeof(struct s_finfo *), sorting);
		/*
		 * end sort()
		 */
		if (options & LS_RECURSE)
		{
			ft_printf("%s:\n", path);
		}
		struct winsize	winsize;
		int				ncol;
		if (options & LS_VERBOSE)
		{
			ft_printf("total %lu\n", tot_blocks / BLOCK_HACK);
		}
		else
		{
			/*
			 * TODO(qsharoly): ls-like column widths
			 */
			retcode = ioctl(0, TIOCGWINSZ, &winsize);
			my_assert(retcode != -1, "ft_ls: failed to get terminal dimensions", Fail_serious);
			ncol = cols.name > 0 ? winsize.ws_col / cols.name : 1;
		}
		i = 0;
		while (i < count)
		{
			if (options & LS_VERBOSE)
			{
				print_verbose_info(infos[i], cols);
			}
			else
			{
				/*
				 * TODO: list by columns
				 */
				//if (options & LS_LIST_BY_LINES_INSTEAD_OF_COLUMNS)
				ft_printf("%-*s", cols.name, infos[i]->name);
				if (0 == (i + 1) % ncol || i == count - 1)
					ft_printf("\n");
			}
			i++;
		}
		i = 0;
		while (i < count)
		{
			if ((options & LS_RECURSE) && (S_ISDIR(infos[i]->status.st_mode))
				&& !ft_strequ(infos[i]->name, ".") && !ft_strequ(infos[i]->name, ".."))
			{
				ft_printf("\n");
				list_path(infos[i]->fullpath, depth + 1, options);
			}
			free(infos[i]->name);
			free(infos[i]->fullpath);
			free(infos[i]->owner);
			free(infos[i]->group);
			free(infos[i]);
			i++;
		}
	}
	else
	{
		if (options & LS_VERBOSE)
		{
			struct s_finfo	info;

			info.name = (char *)path;
			info.fullpath = (char *)path;
			info.status = tmpstat;
			struct passwd	*passwd_data;
			struct group	*group_data;
			passwd_data = getpwuid(info.status.st_uid);
			my_assert(!!passwd_data, "ft_ls: unable to get owner's name", Fail_serious);
			info.owner = ft_strdup(passwd_data->pw_name);
			my_assert(!!info.owner, "ft_ls: malloc failed", Fail_serious);
			group_data = getgrgid(info.status.st_gid);
			my_assert(!!group_data, "ft_ls: unable to get groupname", Fail_serious);
			info.group = ft_strdup(group_data->gr_name);
			my_assert(!!info.group, "ft_ls: malloc failed", Fail_serious);
			cols.lnk = n_digits(info.status.st_nlink);
			cols.size = n_digits(info.status.st_size);
			cols.name = ft_strlen(info.name);
			cols.owner = ft_strlen(info.owner);
			cols.group = ft_strlen(info.group);
			print_verbose_info(&info, cols);
		}
		else
			ft_printf("%s\n", path);
	}
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
