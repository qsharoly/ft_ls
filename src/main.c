/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: debby <marvin@42.fr>                       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/03/10 07:56:32 by debby             #+#    #+#             */
/*   Updated: 2021/10/22 01:25:29 by debby            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_ls.h"
#include "libft.h"
#include "libftprintf.h"
#include <stdlib.h> //malloc(), free(), exit()
#include <fcntl.h> // AT_SYMLINK_NOFOLLOW
#include <sys/stat.h> //struct stat, fstatat()
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
int	(*g_compare)(const void *left, const void *right);

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
			*options |= LS_SHOW_ALL;
		else if (c == 'l')
			*options |= LS_DETAILED;
		else if (c == 'r')
			*options |= LS_SORT_REVERSE;
		else if (c == 'R')
			*options |= LS_RECURSIVE;
		else if (c == 't')
			*options |= LS_SORT_MTIME;
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
	int		pathlen;

	pathlen = ft_strlen(path);
	str = malloc(pathlen + 1 + ft_strlen(name) + 1);
	if (!str)
	{
		panic (Fail_serious, "%s: allocation failed", g_program_name);
	}
	ft_strcpy(str, path);
	while (pathlen > 0 && str[pathlen - 1] == '/')
	{
		str[pathlen - 1] = '\0';
		pathlen--;
	}
	str[pathlen++] = '/';
	ft_strcpy(str + pathlen, name);
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

int		alpha_reverse(const void *l, const void *r)
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

//sort by modification time, newest first, tiebreak alphabetically
int		mtime(const void *l, const void *r)
{
	struct s_finfo	*left;
	struct s_finfo	*right;
	int				diff;

	left = *(struct s_finfo **)l;
	right = *(struct s_finfo **)r;
	diff = right->status->st_mtime - left->status->st_mtime;
	if (diff == 0)
	{
		return (alpha(l, r));
	}
	return (diff);
}

int		mtime_reverse(const void *l, const void *r)
{
	struct s_finfo	*left;
	struct s_finfo	*right;
	int				diff;

	left = *(struct s_finfo **)l;
	right = *(struct s_finfo **)r;
	diff = left->status->st_mtime - right->status->st_mtime;
	if (diff == 0)
	{
		return (alpha_reverse(l, r));
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

static void	print_detailed_info(struct s_finfo	*f, struct s_col_widths w)
{
	mode_t	mode;
	char	perms[11];

	mode = f->status->st_mode;
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
	size_t nlink = f->status->st_nlink;
	size_t fsize = f->status->st_size;
	char *mtime = ctime(&(f->status->st_mtime));
	char *mmm_dd = ft_strchr(mtime, ' ') + 1;
	char *hh_mm = &mtime[11];
	char *_yyyy = &mtime[19];
	char *hm_or_yy;
	time_t now = time(NULL);
#define SIX_MONTHS_AS_SECONDS 15778476
	if (now < f->status->st_mtime || now - f->status->st_mtime > SIX_MONTHS_AS_SECONDS)
	{
		hm_or_yy = _yyyy;
	}
	else
	{
		hm_or_yy = hh_mm;
	}
	ft_printf("%s %*lu %-*s %-*s %*lu %.6s %.5s %s", perms, w.nlink - 1, nlink, w.owner, f->owner, w.group, f->group, w.size - 1, fsize, mmm_dd, hm_or_yy, f->name);
	if (S_ISLNK(mode))
	{
		ft_printf(" -> %.*s\n", f->linklen, f->linkbuf);
	}
	else
		ft_printf("\n");
}

static int	get_termwidth(void)
{
	struct winsize	winsize;
	int				ok;

	ok = ioctl(0, TIOCGWINSZ, &winsize);
	if (ok	< 0)
	{
		panic(Fail_serious, "%s: failed to get terminal dimensions", g_program_name);
	}
	return winsize.ws_col;
}

static bool	print_columnized(struct s_finfo **items, int item_count,
		const int *column_widths, int n_columns, const char *separator)
{
	bool had_printed = false;
	int height = item_count / n_columns + (item_count % n_columns > 0);
	int row = 0;
	while (row < height)
	{
		int	start = row;
		int	step = height;
		int	idx = start;
		int col = 0;
		while (idx < item_count)
		{
			if (idx + step < item_count)
				ft_printf("%-*s%s", column_widths[col], items[idx]->name, separator); 
			else
				ft_printf("%s\n", items[idx]->name);
			col++;
			idx += step;
			had_printed = true;
		}
		row++;
	}
	return had_printed;
}

static void add_owner_and_group(struct s_finfo *info)
{
	struct passwd	*tmp_passwd;
	struct group	*tmp_group;

	tmp_passwd = getpwuid(info->status->st_uid);
	if (!tmp_passwd)
	{
		panic(Fail_serious, "%s: unable to get owner's name", g_program_name);
	}
	info->owner = ft_strdup(tmp_passwd->pw_name);
	if (!info->owner)
	{
		panic(Fail_serious, "%s: allocation failed", g_program_name);
	}
	tmp_group = getgrgid(info->status->st_gid);
	if (!tmp_group)
	{
		panic(Fail_serious, "%s: unable to get groupname", g_program_name);
	}
	info->group = ft_strdup(tmp_group->gr_name);
	if (!info->group)
	{
		panic(Fail_serious, "%s: allocation failed", g_program_name);
	}
}

static void update_detail_column_widths(struct s_col_widths *width_of, struct s_finfo *info)
{
	width_of->nlink = ft_max(width_of->nlink, n_digits(info->status->st_nlink));
	width_of->size = ft_max(width_of->size, n_digits(info->status->st_size));
	width_of->owner = ft_max(width_of->owner, ft_strlen(info->owner));
	width_of->group = ft_max(width_of->group, ft_strlen(info->group));
	width_of->name = ft_max(width_of->name, ft_strlen(info->name));
}

void	list_initial_paths(const char **paths, int path_count, int options)
{
	struct s_finfo		*nondirs[MAX_BREADTH];
	struct s_finfo		*dirs[MAX_BREADTH];
	int					nondir_count;
	int					dir_count;
	int					ok;
	size_t				tot_blocks;
	struct s_col_widths	detail_widths;
	int					i;

	tot_blocks = 0;
	nondir_count = 0;
	dir_count = 0;
	detail_widths.nlink = 0;
	detail_widths.size = 0;
	detail_widths.name = 0;
	detail_widths.owner = 0;
	detail_widths.group = 0;

	// read file information
	i = 0;
	while (i < path_count)
	{
		struct s_finfo	*new_info;
		new_info = ft_calloc(1, sizeof(struct s_finfo));
		if (!new_info)
		{
			panic(Fail_serious, "%s: allocation failed", g_program_name);
		}
		new_info->status = malloc(sizeof(struct stat));
		//in detailed mode print info about links themselves
		if (options & LS_DETAILED)
			ok = lstat(paths[i], new_info->status);
		else
			ok = stat(paths[i], new_info->status);
		if (ok == -1)
		{
			ft_dprintf(STDERR, "%s: cannot access '%s': %s\n", g_program_name, paths[i], strerror(errno));
			free(new_info);
			i++;
			g_had_minor_errors = true;
			continue;
		}
		new_info->name = (char *)paths[i];
		new_info->namelen = ft_strlen(new_info->name);
		if (options & LS_DETAILED)
		{
			new_info->linklen = readlink(new_info->name, new_info->linkbuf, 256);
			add_owner_and_group(new_info);
			update_detail_column_widths(&detail_widths, new_info);
		}
		if (S_ISDIR(new_info->status->st_mode))
		{
			dirs[dir_count++] = new_info;
			i++;
			continue;
		}
		nondirs[nondir_count++] = new_info;
		i++;
	}

	// sort
	qsort(nondirs, nondir_count, sizeof(struct s_finfo *), g_compare);
	qsort(dirs, dir_count, sizeof(struct s_finfo *), g_compare);

	// fit columnized output to terminal width
	int		n_columns = 1;
	int		*column_widths = NULL;
	char	*separator = "  ";
	if (nondir_count > 1 && !(options & LS_SINGLE_COLUMN) && !(options & LS_DETAILED))
	{
		int	termwidth = get_termwidth();
		columnize(&column_widths, &n_columns, nondirs, nondir_count, ft_strlen(separator), termwidth);
	}

	//do we need to print a newline before the first dir announcement?
	bool	had_printed = false;
	//list files
	if (options & LS_DETAILED)
	{
		i = 0;
		while (i < nondir_count)
		{
			print_detailed_info(nondirs[i], detail_widths);
			i++;
			had_printed = true;
		}
	}
	else if (options & LS_SINGLE_COLUMN || n_columns == 1)
	{
		i = 0;
		while (i < nondir_count)
		{
			ft_printf("%s\n", nondirs[i]->name);
			i++;
			had_printed = true;
		}
	}
	else
	{
		had_printed = print_columnized(nondirs, nondir_count, column_widths, n_columns, separator);
	}
	free(column_widths);

	// step into directories
	i = 0;
	while (i < dir_count)
	{
		// don't go into links
		if (S_ISLNK(dirs[i]->status->st_mode))
		{
			i++;
			continue;
		}
		if (options & LS_RECURSIVE || path_count > 1)
		{
			if (had_printed)
				ft_printf("\n");
			ft_printf("%s:\n", dirs[i]->name);
			had_printed = true;
		}
		list_directory(dirs[i]->name, STARTING_DEPTH + 1, options);
		i++;
	}

	i = 0;
	while (i < nondir_count)
	{
		free(nondirs[i]->status);
		free(nondirs[i]->owner);
		free(nondirs[i]->group);
		free(nondirs[i]);
		i++;
	}
	i = 0;
	while (i < dir_count)
	{
		free(dirs[i]->status);
		free(dirs[i]->owner);
		free(dirs[i]->group);
		free(dirs[i]);
		i++;
	}
}

int	scan_directory(struct s_finfo **infos, const char *path, int depth, unsigned options)
{
	DIR				*dir;
	struct dirent	*entry;
	int				ent_count;
	int				ok;

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
	ent_count = 0;
	while ((entry = readdir(dir)))
	{
		if (ent_count >= MAX_BREADTH)
		{
			panic(Fail_serious, "%s: can't list '%s': reached max number of entries per directory.\n", g_program_name, path);
		}
		//skip invisible files
		if (entry->d_name[0] == '.' && !(options & LS_SHOW_ALL))
		{
			continue;
		}
		struct s_finfo	*info = ft_calloc(1, sizeof(*info));
		if (!info)
		{
			panic(Fail_serious, "%s: allocation failed: ", g_program_name);
		}
		if (options & LS_DETAILED || options & LS_SORT_MTIME)
		{
			struct stat	*status = malloc(sizeof(*status));
			if (!status)
			{
				panic(Fail_serious, "%s: allocation failed: ", g_program_name);
			}
			ok = fstatat(dirfd(dir), entry->d_name, status, AT_SYMLINK_NOFOLLOW);
			if (ok == -1)
			{
				ft_dprintf(STDERR, "%s: cannot access '%s/%s': %s\n", g_program_name, path, entry->d_name, strerror(errno));
				free(status);
				free(info);
				g_had_minor_errors = true;
				continue;
			}
			info->status = status;
		}
		if (options & LS_DETAILED)
		{
			info->linklen = readlinkat(dirfd(dir), entry->d_name, info->linkbuf, 256);
		}
		info->name = ft_strdup(entry->d_name);
		info->namelen = ft_strlen(entry->d_name);
		info->is_dir = (entry->d_type == DT_DIR);
		infos[ent_count++] = info;
	}
	closedir(dir);
	return ent_count;
}

void	list_directory(const char *dir_name, int depth, int options)
{
	struct s_finfo		*infos[MAX_BREADTH];
	int					info_count;
	size_t				tot_blocks;
	struct s_col_widths	detail_widths;
	int					i;

	info_count = scan_directory(infos, dir_name, depth, options);
	if (info_count < 0) {
		return;
	}

	// get additional details
	if (options & LS_DETAILED)
	{
		tot_blocks = 0;
		detail_widths.nlink = 0;
		detail_widths.size = 0;
		detail_widths.name = 0;
		detail_widths.owner = 0;
		detail_widths.group = 0;

		i = 0;
		while (i < info_count)
		{
			add_owner_and_group(infos[i]);
			update_detail_column_widths(&detail_widths, infos[i]);
			tot_blocks += infos[i]->status->st_blocks;
			i++;
		}
	}

	// sort
	qsort(infos, info_count, sizeof(struct s_finfo *), g_compare);

	// fit columnized output to terminal width
	int		n_columns = 1;
	int		*column_widths = NULL;
	char	*separator = "  ";
	if (info_count > 1 && !(options & LS_SINGLE_COLUMN) && !(options & LS_DETAILED))
	{
		int	termwidth = get_termwidth();
		columnize(&column_widths, &n_columns, infos, info_count, ft_strlen(separator), termwidth);
	}

	// do we need to print a newline before first dir
	bool	had_printed = false;

	// list files
	if (options & LS_DETAILED)
	{
		ft_printf("total %lu\n", tot_blocks / BLOCK_HACK);
		i = 0;
		while (i < info_count)
		{
			print_detailed_info(infos[i], detail_widths);
			had_printed = true;
			i++;
		}
	}
	else if (options & LS_SINGLE_COLUMN || n_columns == 1)
	{
		i = 0;
		while (i < info_count)
		{
			ft_printf("%s\n", infos[i]->name);
			had_printed = true;
			i++;
		}
	}
	else
	{
		had_printed = print_columnized(infos, info_count, column_widths, n_columns, separator);
	}
	free(column_widths);

	// step into directories
	if (options & LS_RECURSIVE)
	{
		i = 0;
		while (i < info_count)
		{
			if (infos[i]->is_dir
					&& !ft_strequ(infos[i]->name, ".")
					&& !ft_strequ(infos[i]->name, ".."))
			{
				char *sub_dir_path = patcat(dir_name, infos[i]->name);
				if (had_printed)
					ft_printf("\n");
				ft_printf("%s:\n", sub_dir_path);
				had_printed = true;
				list_directory(sub_dir_path, depth + 1, options);
				free(sub_dir_path);
			}
			i++;
		}
	}

	i = 0;
	while (i < info_count)
	{
		free(infos[i]->name);
		free(infos[i]->status);
		free(infos[i]->owner);
		free(infos[i]->group);
		free(infos[i]);
		i++;
	}
}

int		main(int argc, const char **argv)
{
	unsigned	options;
	int			i;
	const char	*paths[MAX_BREADTH];
	int			path_count;

	g_program_name = argv[0];
	options = parse_options(argc, argv);
	// choose comparison function for sorting
	int (*sort_style[4])(const void *left, const void *right) = {
		alpha, alpha_reverse, mtime, mtime_reverse
	};
	g_compare = sort_style[!!(options & LS_SORT_REVERSE) + 2 * !!(options & LS_SORT_MTIME)];
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
			if (path_count >= MAX_BREADTH)
			{
				panic(Fail_serious, "%s: too many file paths specified. exiting.\n", g_program_name);
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
	list_initial_paths(paths, path_count, options);
	if (g_had_minor_errors)
		return (Fail_minor);
	return (Success);
}
