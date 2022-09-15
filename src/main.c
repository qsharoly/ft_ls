/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: debby <marvin@42.fr>                       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/03/10 07:56:32 by debby             #+#    #+#             */
/*   Updated: 2022/09/15 10:42:29 by debby            ###   ########.fr       */
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
#include <limits.h> //PATH_MAX

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

static bool	parse_as_option(const char *str, t_options *options)
{
	int		j;
	char	c;

	if (str[0] != '-')
		return false;
	if (str[1] == '-')
	{
		if (ft_strequ(str, "--help"))
		{
			print_help();
			exit(Success);
		}
		else
		{
			ft_dprintf(STDERR, "%s: unrecognized option '%s'\n", g_program_name, str);
			ft_dprintf(STDERR, "Try '%s --help' for more information.\n", g_program_name);
			exit(Fail_serious);
		}
	}
	j = 1; //skip first dash
	while ((c = str[j]))
	{
		if (c == 'a')
			(*options).show_hidden_files = true;
		else if (c == 'l')
			(*options).detailed_mode = true;
		else if (c == 'r')
			(*options).reverse_sort = true;
		else if (c == 'R')
			(*options).recursive = true;
		else if (c == 't')
			(*options).sort_by_mtime = true;
		else if (c == '1')
			(*options).single_column = true;
		else
		{
			ft_dprintf(STDERR, "%s: invalid option -- '%c'\n", g_program_name, c);
			ft_dprintf(STDERR, "Try '%s --help' for more information.\n", g_program_name);
			exit(Fail_serious);
		}
		j++;
	}
	return true;
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


void path_push(char *path, const char *name)
{
	int pathlen;

	pathlen = ft_strlen(path);
	// remove all trailing slashes
	while (pathlen > 0 && path[pathlen - 1] == '/')
	{
		path[pathlen - 1] = '\0';
		pathlen--;
	}
	// add a single slash
	path[pathlen++] = '/';
	// append the name
	ft_strcpy(path + pathlen, name);
}

void path_pop(char *path)
{
	int pathlen;

	pathlen = ft_strlen(path);
	while (pathlen > 0 && path[pathlen - 1] != '/')
	{
		pathlen--;
	}
	path[pathlen - 1] = '\0';
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
	return (diff || alpha(l, r));
}

int		mtime_reverse(const void *l, const void *r)
{
	struct s_finfo	*left;
	struct s_finfo	*right;
	int				diff;

	left = *(struct s_finfo **)l;
	right = *(struct s_finfo **)r;
	diff = left->status->st_mtime - right->status->st_mtime;
	return (diff || alpha_reverse(l, r));
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
	if (S_ISLNK(mode))
	{
		ft_printf("%s %*lu %-*s %-*s %*lu %.6s %.5s %s -> %.*s\n",
				perms,
				w.nlink - 1, nlink,
				w.owner, f->owner,
				w.group, f->group,
				w.size - 1, fsize,
				mmm_dd, hm_or_yy,
				f->name,
				f->linklen, f->linkbuf);
	}
	else
	{
		ft_printf("%s %*lu %-*s %-*s %*lu %.6s %.5s %s\n",
				perms,
				w.nlink - 1, nlink,
				w.owner, f->owner,
				w.group, f->group,
				w.size - 1, fsize,
				mmm_dd, hm_or_yy,
				f->name);
	}
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

bool	print_informations(struct s_finfo **items, int item_count, t_options options, struct s_width detail_meta_w)
{
	// need to print a newline before the first dir announcement?
	bool	had_printed = false;

	// do printing
	if (options.detailed_mode)
	{
		int i = 0;
		while (i < item_count)
		{
			print_detailed_info(items[i], detail_meta_w);
			i++;
			had_printed = true;
		}
	}
	else if (options.single_column)
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
		if (item_count > 1)
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
				int dirfd, const char *dir_path, t_options options, int statflags)
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
	if (options.detailed_mode || options.sort_by_mtime || dir_path == NULL)
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
	if (options.detailed_mode)
	{
		info->linklen = readlinkat(dirfd, filename, info->linkbuf, 256);
		add_owner_and_group(info);
		update_detail_meta(detail_meta, info);
	}
	return info;
}

void	list_initial_paths(const char **paths, int path_count, t_options options)
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
		if (options.detailed_mode)
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
			dirs[dir_count++] = new_info;
		else
			nondirs[nondir_count++] = new_info;
		i++;
	}

	// sort
	qsort(nondirs, nondir_count, sizeof(struct s_finfo *), g_compare);
	qsort(dirs, dir_count, sizeof(struct s_finfo *), g_compare);


	// print
	bool had_printed = print_informations(nondirs, nondir_count, options, detail_meta.w);

	// list directories (maybe recursively)
	i = 0;
	while (i < dir_count)
	{
		// don't go into links
		if (S_ISLNK(dirs[i]->status->st_mode))
		{
			i++;
			continue;
		}
		if (options.recursive || path_count > 1)
		{
			if (had_printed)
				ft_printf("\n");
			ft_printf("%s:\n", dirs[i]->name);
			had_printed = true;
		}
		char path_buffer[PATH_MAX] = {};
		ft_strcpy(path_buffer, dirs[i]->name);
		list_directory(path_buffer, STARTING_DEPTH + 1, options);
		i++;
	}

	destroy_infos(nondirs, nondir_count);
	destroy_infos(dirs, dir_count);
}

void	list_directory(char *path_buffer, int depth, t_options options)
{
	struct s_finfo	*infos[MAX_BREADTH];
	int				info_count;
	struct s_meta	detail_meta = {0};

	// read files from directory
	const char		*dir_name = path_buffer;
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
		if (entry->d_name[0] == '.' && !options.show_hidden_files)
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
	if (options.detailed_mode)
	{
		ft_printf("total %lu\n", detail_meta.total_blocks / BLOCK_HACK);
	}
	bool had_printed = print_informations(infos, info_count, options, detail_meta.w);

	// step into directories
	if (options.recursive)
	{
		int i = 0;
		while (i < info_count)
		{
			if (infos[i]->is_dir
					&& !ft_strequ(infos[i]->name, ".")
					&& !ft_strequ(infos[i]->name, ".."))
			{
				path_push(path_buffer, infos[i]->name);
				if (had_printed)
					ft_printf("\n");
				ft_printf("%s:\n", path_buffer);
				had_printed = true;
				list_directory(path_buffer, depth + 1, options);
				path_pop(path_buffer);
			}
			i++;
		}
	}

	destroy_infos(infos, info_count);
}

int		main(int argc, const char **argv)
{
	t_options	options;
	const char	*paths[MAX_BREADTH];
	int			path_count;
	int 		(*cmp_select[2][2])(const void *left, const void *right) = {
		{ alpha, mtime }, { alpha_reverse, mtime_reverse }
	};

	g_program_name = argv[0];
	options = (t_options){0};
	path_count = 0;
	if (argc > 1)
	{
		int i = 1;
		while (i < argc)
		{
			if (parse_as_option(argv[i], &options))
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
	g_compare = cmp_select[options.reverse_sort][options.sort_by_mtime];
	list_initial_paths(paths, path_count, options);
	if (g_had_minor_errors)
		return (Fail_minor);
	return (Success);
}
