/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: debby <marvin@42.fr>                       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/03/10 07:56:32 by debby             #+#    #+#             */
/*   Updated: 2021/10/08 20:17:21 by debby            ###   ########.fr       */
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

#include <stdbool.h>
static bool	g_had_minor_errors = false;
const char	*g_program_name = "ft_ls"; //initialize to default name

static int	scan_dir(char **sub_paths, const char *path, int depth, unsigned options);

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

static void	parse_option(const char *str, unsigned *options)
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
			*options |= LS_DETAILED;
		else if (c == 'r')
			*options |= LS_SORT_REVERSE;
		else if (c == 'R')
			*options |= LS_RECURSIVE;
		else if (c == 't')
			*options |= LS_SORT_BY_TIME;
		else if (c == 'x')
			*options |= LS_TRANSPOSE_COLUMNS;
		else if (c == '1')
			*options |= LS_SINGLE_COLUMN;
		else
		{
			ft_dprintf(STDERR, "%s: invalid option -- '%c'\n", g_program_name, c);
			ft_dprintf(STDERR, "Try '%s --help' for more information.\n", g_program_name);
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
		{
			parse_option(&argv[i][1], &options);
		}
		i++;
	}
	return (options);
}


__attribute__((__format__(__printf__, 2, 3)))
void	panic(enum e_exitcode status, const char *message_fmt, ...)
{
	va_list	ap;

	va_start(ap, message_fmt);
	ft_vdprintf(STDERR, message_fmt, ap);
	va_end(ap);
	perror("");
	exit(status);
}


char	*patcat(const char *path, const char *name)
{
	char	*str;

	str = malloc(ft_strlen(path) + 1 + ft_strlen(name) + 1);
	if (!str)
	{
		panic (Fail_serious, "%s: allocation failed", g_program_name);
	}
	ft_strcpy(str, path);
	if (!ft_strequ(path, "/"))
	{
		ft_strcat(str, "/");
	}
	ft_strcat(str, name);
	return (str);
}

int		alpha(const void *l, const void *r)
{
	struct s_finfo	*left;
	struct s_finfo	*right;
	const char		*lname;
	const char		*rname;

	left = *(struct s_finfo **)l;
	right = *(struct s_finfo **)r;
	lname = left->name;
	rname = right->name;
	if (lname[0] == '.')
		lname++;
	if (rname[0] == '.')
		rname++;
	return (ft_stricmp(lname, rname));
}

int		rev_alpha(const void *l, const void *r)
{
	struct s_finfo	*left;
	struct s_finfo	*right;
	const char		*lname;
	const char		*rname;

	left = *(struct s_finfo **)l;
	right = *(struct s_finfo **)r;
	lname = left->name;
	rname = right->name;
	if (lname[0] == '.')
		lname++;
	if (rname[0] == '.')
		rname++;
	return (-ft_stricmp(lname, rname));
}

//by modification time, newest first, tibreak alphabetically
int		mtime(const void *l, const void *r)
{
	struct s_finfo	*left;
	struct s_finfo	*right;
	int				diff;

	left = *(struct s_finfo **)l;
	right = *(struct s_finfo **)r;
	diff = right->status.st_mtime - left->status.st_mtime;
	if (diff == 0)
	{
		return (alpha(l, r));
	}
	return (diff);
}

int		rev_mtime(const void *l, const void *r)
{
	struct s_finfo	*left;
	struct s_finfo	*right;
	int				diff;

	left = *(struct s_finfo **)l;
	right = *(struct s_finfo **)r;
	diff = left->status.st_mtime - right->status.st_mtime;
	if (diff == 0)
	{
		return (rev_alpha(l, r));
	}
	return (diff);
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

static void	print_detailed_info(struct s_finfo	*f, struct s_col_widths cols)
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
#define SIX_MONTHS_AS_SECONDS 15778476
	if (now < f->status.st_mtime || now - f->status.st_mtime > SIX_MONTHS_AS_SECONDS)
	{
		hm_or_yy = _yyyy;
	}
	else
	{
		hm_or_yy = hh_mm;
	}
	ft_printf("%s %*lu %-*s %-*s %*lu %.6s %.5s %-s", perms, cols.lnk - 1, nlink, cols.owner, f->owner, cols.group, f->group, cols.size - 1, fsize, mmm_dd, hm_or_yy, f->name);
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

void	list_paths(const char **paths, int path_count, int depth, int options)
{
	struct s_finfo		*infos[MAX_WIDTH];
	const char			*basename;
	int					i;
	int					tmp_res;
	size_t				tot_blocks;
	struct s_col_widths	cols;
	int					info_count;
	int					nondirs_remaining;

	tot_blocks = 0;
	cols.lnk = 0;
	cols.size = 0;
	cols.name = 0;
	cols.owner = 0;
	cols.group = 0;
	info_count = 0;
	nondirs_remaining = 0;
	i = 0;
	while (i < path_count)
	{
		basename = ft_strrchr(paths[i], '/');
		//don't truncate paths from argv
		if (!basename || depth == STARTING_DEPTH)
			basename = paths[i];
		else
			basename += 1;
		//skip invisible files except for those from argv
		if (basename[0] == '.' && !(options & LS_LIST_ALL || depth == STARTING_DEPTH))
		{
			i++;
			continue;
		}
		struct s_finfo	*new_info;
		new_info = (struct s_finfo *)ft_calloc(1, sizeof(struct s_finfo));
		if (!new_info)
		{
			panic(Fail_serious, "%s: allocation failed", g_program_name);
		}
		//dont go into links in detailed mode
		if (options & LS_DETAILED)
		{
			tmp_res = lstat(paths[i], &new_info->status);
		}
		else
		{
			tmp_res = stat(paths[i], &new_info->status);
		}
		if (tmp_res == -1)
		{
			ft_dprintf(STDERR, "%s: cannot access '%s': %s\n", g_program_name, paths[i], strerror(errno));
			free(new_info);
			i++;
			g_had_minor_errors = true;
			continue;
		}
		new_info->fullname = paths[i];
		new_info->name = basename;
		if (options & LS_DETAILED)
		{
			struct passwd	*tmp_passwd;
			struct group	*tmp_group;
			tmp_passwd = getpwuid(new_info->status.st_uid);
			if (!tmp_passwd)
			{
				panic(Fail_serious, "%s: unable to get owner's name", g_program_name);
			}
			new_info->owner = ft_strdup(tmp_passwd->pw_name);
			if (!new_info->owner)
			{
				panic(Fail_serious, "%s: allocation failed", g_program_name);
			}
			tmp_group = getgrgid(new_info->status.st_gid);
			if (!tmp_group)
			{
				panic(Fail_serious, "%s: unable to get groupname", g_program_name);
			}
			new_info->group = ft_strdup(tmp_group->gr_name);
			if (!new_info->group)
			{
				panic(Fail_serious, "%s: allocation failed", g_program_name);
			}
			cols.lnk = ft_max(cols.lnk, n_digits(new_info->status.st_nlink));
			cols.size = ft_max(cols.size, n_digits(new_info->status.st_size));
			cols.owner = ft_max(cols.owner, ft_strlen(new_info->owner));
			cols.group = ft_max(cols.group, ft_strlen(new_info->group));
		}
		cols.name = ft_max(cols.name, ft_strlen(new_info->name));
		tot_blocks += new_info->status.st_blocks;
		infos[info_count] = new_info;
		if (!S_ISDIR(new_info->status.st_mode))
			nondirs_remaining++;
		info_count++;
		i++;
	}
	int	(*compare)(const void *left, const void *right);
	/*
	// too clever:
	int (*sort_style[4])(const void *left, const void *right) = {
		alpha, alpha_reverse, mtime, mtime_reverse
	};
	compare = sort_style[!!(options & LS_SORT_REVERSE) + 2*!!(options & LS_SORT_BY_TIME)];
	*/
	if (options & LS_SORT_REVERSE)
	{
		compare = rev_alpha;
		if (options & LS_SORT_BY_TIME)
		{
			compare = rev_mtime;
		}
	}
	else
	{
		compare = alpha;
		if (options & LS_SORT_BY_TIME)
		{
			compare = mtime;
		}
	}
	qsort(infos, info_count, sizeof(struct s_finfo *), compare);
	if (options & LS_DETAILED && depth > STARTING_DEPTH)
	{
		ft_printf("total %lu\n", tot_blocks / BLOCK_HACK);
	}
	int		ncol;
	ncol = 0;
	/*
	struct winsize	winsize;
	if (!(options & LS_SINGLE_COLUMN || options & LS_DETAILED))
	{
		// TODO(qsharoly): ls-like column widths
		tmp_res = ioctl(0, TIOCGWINSZ, &winsize);
		gate(!(tmp_res >= 0), "ft_ls: failed to get terminal dimensions", Fail_serious);
		if (cols.name > 0 && winsize.ws_col > cols.name)
			ncol = winsize.ws_col / cols.name;
	}
	*/
	//flag to skip newline before first dir
	int	had_printed = 0;
	//list files
	i = 0;
	while (i < info_count)
	{
		//skip paths from argv which are dirs;
		if (S_ISDIR(infos[i]->status.st_mode) && depth == STARTING_DEPTH)
		{
			i++;
			continue;
		}
		if (options & LS_DETAILED)
		{
			print_detailed_info(infos[i], cols);
		}
		else if (options & LS_SINGLE_COLUMN || ncol == 1)
		{
			ft_printf("%s\n", infos[i]->name);
		}
		else
		{
			if (i == info_count - 1 || (depth == STARTING_DEPTH && --nondirs_remaining == 0))
				ft_printf("%s\n", infos[i]->name);
			else
				ft_printf("%s  ", infos[i]->name);
		}
		/*
		//TODO: add transposed column output

		ft_printf("%-*s", cols.name, infos[i]->name);
		if (0 == (i + 1) % ncol || i == info_count - 1)
			ft_printf("\n");
		*/
		had_printed += 1;
		i++;
	}
	//step into directories
	if (options & LS_RECURSIVE || depth == STARTING_DEPTH)
	{
		i = 0;
		while (i < info_count)
		{
			//go into "." only from argv
			if (depth > STARTING_DEPTH && ft_strequ(infos[i]->name, "."))
			{
				i++;
				continue;
			}
			//go into ".." only from argv
			if (depth > STARTING_DEPTH && ft_strequ(infos[i]->name, ".."))
			{
				i++;
				continue;
			}
			//don't go into links
			if (S_ISLNK(infos[i]->status.st_mode))
			{
				i++;
				continue;
			}
			if (S_ISDIR(infos[i]->status.st_mode))
			{
				char	*sub_paths[MAX_WIDTH];
				int		sub_count;
				sub_count = scan_dir(sub_paths, infos[i]->fullname, depth + 1, options);
				if (sub_count < 0) {
					i++;
					continue;
				}
				if (had_printed == 0)
					had_printed += 1;
				else
					ft_printf("\n");
				if (depth > STARTING_DEPTH || options & LS_RECURSIVE || (depth == STARTING_DEPTH && path_count > 1))
					ft_printf("%s:\n", infos[i]->fullname);
				list_paths((const char **)sub_paths, sub_count, depth + 1, options);
				int j = 0;
				while (j < sub_count)
				{
					free(sub_paths[j]);
					j++;
				}
			}
			i++;
		}
	}
	i = 0;
	while (i < info_count)
	{
		free(infos[i]->owner);
		free(infos[i]->group);
		free(infos[i]);
		i++;
	}
}

static int	scan_dir(char **sub_paths, const char *path, int depth, unsigned options)
{
	DIR				*dir;
	struct dirent	*entry;
	int				sub_count;

	(void)options;
	if (depth >= MAX_DEPTH)
	{
		panic(Fail_serious, "%s: can't list '%s': reached max directory depth.\n", g_program_name, path);
	}
	dir = opendir(path);
	if (!dir)
	{
		ft_dprintf(STDERR, "%s: cannot open directory '%s': %s\n", g_program_name, path, strerror(errno));
		g_had_minor_errors = true;
		return -1;
	}
	sub_count = 0;
	while ((entry = readdir(dir)))
	{
		if (sub_count >= MAX_WIDTH)
		{
			panic(Fail_serious, "%s: can't list '%s': reached max number of entries.\n", g_program_name, path);
		}
		sub_paths[sub_count] = patcat(path, entry->d_name);
		sub_count++;
	}
	closedir(dir);
	return sub_count;
}

int		main(int argc, const char **argv)
{
	unsigned	options;
	int			i;
	const char	*paths[MAX_WIDTH];
	int			path_count;

	g_program_name = argv[0];
	options = parse_options(argc, argv);
	path_count = 0;
	if (argc > 1)
	{
		i = 1;
		while (i < argc)
		{
			if (argv[i][0] == '-')
			{
				i++;
				continue;
			}
			if (path_count >= MAX_WIDTH)
			{
				panic(Fail_serious, "%s: too many files. exiting.\n", g_program_name);
			}
			paths[path_count] = argv[i];
			path_count++;
			i++;
		}
	}
	if (path_count == 0)
	{
		path_count = 1;
		paths[0] = ".";
	}
	list_paths(paths, path_count, STARTING_DEPTH, options);
	if (g_had_minor_errors)
	{
		return (Fail_minor);
	}
	else
	{
		return (Success);
	}
}
