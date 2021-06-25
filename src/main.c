/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: debby <marvin@42.fr>                       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/03/10 07:56:32 by debby             #+#    #+#             */
/*   Updated: 2021/06/25 11:10:22 by debby            ###   ########.fr       */
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
#include <unistd.h> //readlink()

static void	list_dir(const char *path, int depth, unsigned options);

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

static void	parse_option(const char *str, unsigned *options, const char *av0)
{
	int		j;
	char	c;

	if (ft_strequ(str - 1, "--help"))
	{
		print_help();
		exit(Success);
	}
	j = 0;
	while ((c = str[j]))
	{
		if (c == 'a')
			*options |= LS_LIST_ALL;
		else if (c == 'l')
			*options |= LS_VERBOSE;
		else if (c == 'r')
			*options |= LS_SORT_REVERSE;
		else if (c == 'R')
			*options |= LS_RECURSIVE;
		else if (c == 't')
			*options |= LS_SORT_BY_TIME;
		else if (c == 'x')
			*options |= LS_LIST_BY_LINES_INSTEAD_OF_COLUMNS;
		else
		{
			ft_printf("%s: invalid option -- '%c'\n", av0, c);
			ft_printf("Try '%s --help' for more information.\n", av0);
			exit(Fail_serious);
		}
		j++;
	}
}

static unsigned parse_options(int argc, const char **argv)
{
	unsigned	options;
	int			i;

	options = 0;
	i = 1;
	while (i < argc)
	{
		if (argv[i][0] == '-')
			parse_option(&argv[i][1], &options, argv[0]);
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

void	gate(int condition, const char *message, enum e_exitcode code)
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
	char *mtime = ctime(&(f->status.st_mtime));
	char *mmm_dd = ft_strchr(mtime, ' ') + 1;
	char *hh_mm = &mtime[11];
	char *_yyyy = &mtime[19];
	char *hm_or_yy;
	time_t now = time(NULL);
#define SIX_MONTHS_IN_SEC 15778476
	if (now > f->status.st_mtime && now - f->status.st_mtime > SIX_MONTHS_IN_SEC)
		hm_or_yy = _yyyy;
	else
		hm_or_yy = hh_mm;
	char *name = f->name;
	ft_printf("%s %*lu %-*s %-*s %*lu %.6s %.5s %-s", perms, cols.lnk - 1, nlink, cols.owner - 1, f->owner, cols.group - 1, f->group, cols.size - 1, fsize, mmm_dd, hm_or_yy, name);
	//TODO: read link
	if (S_ISLNK(mode))
	{
#define LINKBUF 80
		char linkbuf[LINKBUF];
		size_t linklen = readlink(f->fullname, linkbuf, LINKBUF);
		ft_printf(" -> %.*s\n", linklen, linkbuf);
	}
	else
		ft_printf("\n");
}

void	list_paths(char **paths, int path_count, int depth, int options)
{
	struct s_finfo		*infos[MAX_WIDTH];
	char				*name_ptr;
	int					i;
	int					(*compare)(const void *left, const void *right);
	int					tmp_res;
	size_t				tot_blocks;
	struct s_col_widths	cols;

	tot_blocks = 0;
	cols.lnk = 0;
	cols.size = 0;
	cols.name = 0;
	cols.owner = 0;
	cols.group = 0;
	i = 0;
	while (i < path_count)
	{
		infos[i] = (struct s_finfo *)malloc(sizeof(struct s_finfo));
		gate(!!infos[i], "ft_ls: malloc failed", Fail_serious);
		if (options & LS_VERBOSE)
			tmp_res = lstat(paths[i], &infos[i]->status);
		else
			tmp_res = stat(paths[i], &infos[i]->status);
		if (-1 == tmp_res)
		{
			ft_printf("ft_ls: cannot access '%s': %s\n", paths[i], strerror(errno));
			exit(Fail_serious);
		}
		name_ptr = ft_strrchr(paths[i], '/');
		if (!name_ptr)
			name_ptr = paths[i];
		else
			name_ptr += 1;
		infos[i]->name = ft_strdup(name_ptr);
		gate(!!infos[i]->name, "ft_ls: malloc failed", Fail_serious);
		infos[i]->fullname = ft_strdup(paths[i]);
		gate(!!infos[i]->fullname, "ft_ls: malloc failed", Fail_serious);
		struct passwd	*tmp_passwd;
		struct group	*tmp_group;
		tmp_passwd = getpwuid(infos[i]->status.st_uid);
		gate(!!tmp_passwd, "ft_ls: unable to get owner's name", Fail_serious);
		infos[i]->owner = ft_strdup(tmp_passwd->pw_name);
		gate(!!infos[i]->owner, "ft_ls: malloc failed", Fail_serious);
		tmp_group = getgrgid(infos[i]->status.st_gid);
		gate(!!tmp_group, "ft_ls: unable to get groupname", Fail_serious);
		infos[i]->group = ft_strdup(tmp_group->gr_name);
		gate(!!infos[i]->group, "ft_ls: malloc failed", Fail_serious);
		tot_blocks += infos[i]->status.st_blocks;
		cols.lnk = ft_max(cols.lnk, n_digits(infos[i]->status.st_nlink));
		cols.size = ft_max(cols.size, n_digits(infos[i]->status.st_size));
		cols.name = ft_max(cols.name, ft_strlen(infos[i]->name));
		cols.owner = ft_max(cols.owner, ft_strlen(infos[i]->owner));
		cols.group = ft_max(cols.group, ft_strlen(infos[i]->group));
		i++;
	}
	int info_count = i;
	compare = alpha;
	if (options & LS_SORT_BY_TIME)
		compare = mtime;
	if (options & LS_SORT_REVERSE)
	{
		compare = compare == alpha ? rev_alpha : rev_mtime;
	}
	qsort(infos, path_count, sizeof(struct s_finfo *), compare);
	struct winsize	winsize;
	int				ncol;
	if (options & LS_VERBOSE)
	{
		ft_printf("total %lu\n", tot_blocks / BLOCK_HACK);
	}
	else
	{
		// TODO(qsharoly): ls-like column widths
		tmp_res = ioctl(0, TIOCGWINSZ, &winsize);
		gate(tmp_res != -1, "ft_ls: failed to get terminal dimensions", Fail_serious);
		ncol = cols.name > 0 ? winsize.ws_col / cols.name : 1;
	}
	i = 0;
	while (i < info_count)
	{
		//skip invisible files
		if (infos[i]->name[0] == '.' && !(options & LS_LIST_ALL))
		{
			i++;
			continue;
		}
		//skip paths from argv which are dirs;
		if (S_ISDIR(infos[i]->status.st_mode) && depth == STARTING_DEPTH)
		{
			i++;
			continue;
		}
		if (options & LS_VERBOSE)
		{
			print_verbose_info(infos[i], cols);
		}
		else
		{
			//TODO: add transposed column output

			ft_printf("%-*s", cols.name, infos[i]->name);
			if (0 == (i + 1) % ncol || i == info_count - 1)
				ft_printf("\n", i);
		}
		i++;
	}
	//list directories
	if (options & LS_RECURSIVE || depth == STARTING_DEPTH)
	{
		i = 0;
		while (i < path_count)
		{
			if (depth > STARTING_DEPTH && ft_strequ(infos[i]->name, "."))
			{
				i++;
				continue;
			}
			if (ft_strequ(infos[i]->name, ".."))
			{
				i++;
				continue;
			}
			if (S_ISDIR(infos[i]->status.st_mode))
				list_dir(infos[i]->fullname, depth, options);
			i++;
		}
	}
	i = 0;
	while (i < path_count)
	{
		free(infos[i]->name);
		free(infos[i]->fullname);
		free(infos[i]->owner);
		free(infos[i]->group);
		free(infos[i]);
		i++;
	}
}

static void	list_dir(const char *path, int depth, unsigned options)
{
	DIR				*dir;
	struct dirent	*entry;
	char			*sub_paths[MAX_WIDTH];
	int				sub_count;

	if (depth > MAX_DEPTH)
	{
		ft_printf("can't list '%s': reached max directory depth.\n", path);
		exit(Fail_minor);
	}
	dir = opendir(path);
	sub_count = 0;
	while ((entry = readdir(dir)))
	{
		if (sub_count > MAX_WIDTH)
		{
			ft_printf("%s: reached max number of entries. exiting.\n", path);
			exit(Fail_serious);
		}
		sub_paths[sub_count] = patcat(path, entry->d_name);
		sub_count++;
	}
	closedir(dir);
	if (depth > STARTING_DEPTH)
		ft_printf("\n");
	if (depth > STARTING_DEPTH || (options & LS_RECURSIVE))
		ft_printf("%s:\n", path);
	list_paths(sub_paths, sub_count, depth + 1, options);
	int j = 0;
	while (j < sub_count)
	{
		free(sub_paths[j]);
		j++;
	}
}

int		main(int argc, const char **argv)
{
	unsigned	options;
	int			i;
	char		*paths[MAX_WIDTH];
	int			path_count;

	options = parse_options(argc, argv);
	if (argc > 1)
	{
		path_count = 0;
		i = 1;
		while (i < argc)
		{
			if (argv[i][0] == '-')
			{
				i++;
				continue;
			}
			gate(path_count < MAX_WIDTH, "ft_ls: too many files. exiting.\n", Fail_serious);
			paths[path_count] = ft_strdup(argv[i]);
			path_count++;
			i++;
		}
	}
	if (path_count > 0)
		list_paths(paths, path_count, STARTING_DEPTH, options);
	else
		list_dir(".", STARTING_DEPTH, options);
	return (0);
}
