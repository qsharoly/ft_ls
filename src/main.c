/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: debby <marvin@42.fr>                       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/03/10 07:56:32 by debby             #+#    #+#             */
/*   Updated: 2022/03/29 16:41:03 by debby            ###   ########.fr       */
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

static void	parse_option(const char *str, int *options)
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

__attribute__((__format__(__printf__, 2, 3)))
void	panic(enum e_exitcode status, const char *message_fmt, ...)
{
	va_list	ap;

	va_start(ap, message_fmt);
	ft_vdprintf(STDERR, message_fmt, ap);
	va_end(ap);
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
		panic(Fail_serious, "%s: allocation failed: %s\n", g_program_name, strerror(errno));
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

static void	print_detailed_info(struct s_finfo	*f, struct s_width w)
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
		panic(Fail_serious, "%s: failed to get terminal dimensions: %s\n", g_program_name, strerror(errno));
	}
	return winsize.ws_col;
}

static void add_owner_and_group(struct s_finfo *info)
{
	struct passwd	*tmp_passwd;
	struct group	*tmp_group;

	tmp_passwd = getpwuid(info->status->st_uid);
	if (!tmp_passwd)
	{
		panic(Fail_serious, "%s: unable to get owner's name: %s\n", g_program_name, strerror(errno));
	}
	info->owner = ft_strdup(tmp_passwd->pw_name);
	if (!info->owner)
	{
		panic(Fail_serious, "%s: allocation failed: %s\n", g_program_name, strerror(errno));
	}
	tmp_group = getgrgid(info->status->st_gid);
	if (!tmp_group)
	{
		panic(Fail_serious, "%s: unable to get groupname: %s\n", g_program_name, strerror(errno));
	}
	info->group = ft_strdup(tmp_group->gr_name);
	if (!info->group)
	{
		panic(Fail_serious, "%s: allocation failed: %s\n", g_program_name, strerror(errno));
	}
}

static void update_detail_meta(struct s_meta *meta, struct s_finfo *info)
{
	meta->total_blocks += info->status->st_blocks;
	meta->w.nlink = ft_max(meta->w.nlink, n_digits(info->status->st_nlink));
	meta->w.size = ft_max(meta->w.size, n_digits(info->status->st_size));
	meta->w.owner = ft_max(meta->w.owner, ft_strlen(info->owner));
	meta->w.group = ft_max(meta->w.group, ft_strlen(info->group));
	meta->w.name = ft_max(meta->w.name, ft_strlen(info->name));
}

void	destroy_infos(struct s_finfo **infos, int info_count)
{
	int i = 0;
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

bool	print_informations(struct s_finfo **items, int item_count, int options, struct s_width detail_meta_w)
{
	// need to print a newline before the first dir announcement?
	bool	had_printed = false;

	// do printing
	if (options & LS_DETAILED)
	{
		int i = 0;
		while (i < item_count)
		{
			print_detailed_info(items[i], detail_meta_w);
			i++;
			had_printed = true;
		}
	}
	else if (options & LS_SINGLE_COLUMN)
	{
		int i = 0;
		while (i < item_count)
		{
			ft_printf("%s\n", items[i]->name);
			i++;
			had_printed = true;
		}
	}
	else
	{
		char	*separator = "  ";
		int		*column_widths = NULL;
		int		stride = 1;
		if (item_count > 1 && !(options & LS_SINGLE_COLUMN) && !(options & LS_DETAILED))
		{
			int	termwidth = get_termwidth();
			stride = columnize(&column_widths, items, item_count, ft_strlen(separator), termwidth);
		}
		int row = 0;
		while (row < stride)
		{
			int	start = row;
			int	idx = start;
			int col = 0;
			while (idx < item_count)
			{
				if (idx + stride < item_count)
					ft_printf("%-*s%s", column_widths[col], items[idx]->name, separator); 
				else
					ft_printf("%s\n", items[idx]->name);
				col++;
				idx += stride;
				had_printed = true;
			}
			row++;
		}
		free(column_widths);
	}

	return had_printed;
}

struct s_finfo *get_file_info(const char *filename, struct s_meta *detail_meta,
				int dirfd, const char *dir_path, int options, int statflags)
{
	struct s_finfo	*info;
	int ok;

	info = ft_calloc(1, sizeof(*info));
	if (!info)
	{
		panic(Fail_serious, "%s: allocation failed: %s\n", g_program_name, strerror(errno));
	}
	info->name = ft_strdup(filename);
	info->namelen = ft_strlen(filename);
	//dir_path == NULL means we are called from list_initial_files
	if (options & LS_DETAILED || options & LS_SORT_MTIME || dir_path == NULL)
	{
		struct stat	*status = malloc(sizeof(*status));
		if (!status)
		{
			panic(Fail_serious, "%s: allocation failed: %s\n", g_program_name, strerror(errno));
		}
		ok = fstatat(dirfd, filename, status, statflags);
		if (ok == -1)
		{
			ft_dprintf(STDERR, "%s: cannot access '%s/%s': %s\n", g_program_name, dir_path, filename, strerror(errno));
			free(status);
			free(info->name);
			free(info);
			g_had_minor_errors = true;
			return NULL;
		}
		info->status = status;
	}
	if (options & LS_DETAILED)
	{
		info->linklen = readlinkat(dirfd, filename, info->linkbuf, 256);
		add_owner_and_group(info);
		update_detail_meta(detail_meta, info);
	}
	return info;
}

void	list_initial_paths(const char **paths, int path_count, int options)
{
	struct s_finfo	*nondirs[MAX_BREADTH];
	struct s_finfo	*dirs[MAX_BREADTH];
	int				nondir_count;
	int				dir_count;
	struct s_meta	detail_meta = {0};
	int				i;

	nondir_count = 0;
	dir_count = 0;

	// read file information
	i = 0;
	while (i < path_count)
	{
		struct s_finfo	*new_info;
		//in detailed mode print info about links themselves
		if (options & LS_DETAILED)
			new_info = get_file_info(paths[i], &detail_meta, AT_FDCWD, NULL, options, AT_SYMLINK_NOFOLLOW);
		else
			new_info = get_file_info(paths[i], &detail_meta, AT_FDCWD, NULL, options, 0);
		if (!new_info)
		{
			g_had_minor_errors = true;
			i++;
			continue;
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


	// print
	bool had_printed = print_informations(nondirs, nondir_count, options, detail_meta.w);

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

	destroy_infos(nondirs, nondir_count);
	destroy_infos(dirs, dir_count);
}

void	list_directory(const char *dir_name, int depth, int options)
{
	struct s_finfo	*infos[MAX_BREADTH];
	int				info_count;
	struct s_meta	detail_meta = {0};

	// read files from directory
	DIR				*dir;
	struct dirent	*entry;
	if (depth >= MAX_DEPTH)
	{
		panic(Fail_serious, "%s: can't list '%s': reached max directory depth.\n", g_program_name, dir_name);
	}
	dir = opendir(dir_name);
	if (!dir)
	{
		ft_dprintf(STDERR, "%s: cannot open directory '%s': %s\n", g_program_name, dir_name, strerror(errno));
		g_had_minor_errors = true;
		return;
	}
	info_count = 0;
	while ((entry = readdir(dir)))
	{
		if (info_count >= MAX_BREADTH)
		{
			panic(Fail_serious, "%s: can't list '%s': reached max number of entries per directory.\n", g_program_name, dir_name);
		}
		//skip invisible files
		if (entry->d_name[0] == '.' && !(options & LS_SHOW_ALL))
		{
			continue;
		}
		struct s_finfo *new_info = get_file_info(entry->d_name, &detail_meta, dirfd(dir), dir_name, options, AT_SYMLINK_NOFOLLOW);
		if (!new_info)
		{
			continue;
		}
		new_info->is_dir = (entry->d_type == DT_DIR);
		infos[info_count++] = new_info;
	}
	closedir(dir);

	// sort
	qsort(infos, info_count, sizeof(*infos), g_compare);

	// print
	if (options & LS_DETAILED)
	{
		ft_printf("total %lu\n", detail_meta.total_blocks / BLOCK_HACK);
	}
	bool had_printed = print_informations(infos, info_count, options, detail_meta.w);

	// step into directories
	if (options & LS_RECURSIVE)
	{
		int i = 0;
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

	destroy_infos(infos, info_count);
}

int		main(int argc, const char **argv)
{
	int			options;
	const char	*paths[MAX_BREADTH];
	int			path_count;
	int 		(*sort_by[2][2])(const void *left, const void *right) = {
		{ alpha, mtime }, { alpha_reverse, mtime_reverse }
	};

	g_program_name = argv[0];
	options = 0;
	path_count = 0;
	if (argc > 1)
	{
		int i = 1;
		while (i < argc)
		{
			if (argv[i][0] == '-')
			{
				parse_option(&argv[i][1], &options);
				i++;
				continue;
			}
			if (path_count >= MAX_BREADTH)
			{
				panic(Fail_serious, "%s: too many file paths specified. exiting.\n", g_program_name);
			}
			paths[path_count] = ft_strdup(argv[i]);
			if (!paths[path_count])
			{
				panic(Fail_serious, "%s: allocation failed: %s\n", g_program_name, strerror(errno));
			}
			path_count++;
			i++;
		}
	}
	if (path_count == 0)
	{
		path_count = 1;
		paths[0] = ".";
	}
	// choose comparison function for sorting
	g_compare = sort_by[!!(options & LS_SORT_REVERSE)][!!(options & LS_SORT_MTIME)];
	list_initial_paths(paths, path_count, options);
	if (g_had_minor_errors)
		return (Fail_minor);
	return (Success);
}
