/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: debby <marvin@42.fr>                       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/03/10 07:56:32 by debby             #+#    #+#             */
/*   Updated: 2023/10/17 11:27:45 by kith             ###   ########.fr       */
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

void	list_directory(char *path_buffer, int depth, t_options options,
		bool *had_printed, bool *should_announce,
		t_arena *names_arena, t_arena *infos_arena, t_stream *out);

#define TABLE_SIZE 1024 //must be power of 2 for calc_hash to work
int calc_hash(int key)
{
	return abs(key) & (TABLE_SIZE-1);
}

struct hashNode
{
	int key;
	char *value;
	struct hashNode *next;
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
		else if (ft_strequ(str, "--mem"))
		{
			(*options).mem_usage = true;
			return true;
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
	lname = left->name.start;
	rname = right->name.start;
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
	lname = left->name.start;
	rname = right->name.start;
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
	diff = right->status.st_mtime - left->status.st_mtime;
	return (diff || alpha(l, r));
}

int		mtime_reverse(const void *l, const void *r)
{
	struct s_finfo	*left;
	struct s_finfo	*right;
	int				diff;

	left = *(struct s_finfo **)l;
	right = *(struct s_finfo **)r;
	diff = left->status.st_mtime - right->status.st_mtime;
	return (diff || alpha_reverse(l, r));
}

size_t	n_digits(size_t val)
{
	size_t	n;

	if (val == 0)
	{
		return 1;
	}
	n = 0;
	while (val)
	{
		val /= 10;
		n++;
	}
	return (n);
}

static void put_unsigned_simple(unsigned long long value, t_stream *out)
{
	char				buffer[MAXBUF_UTOA];
	t_sv				repr;
	int					base = 10;
	int					upcase = 0;

	repr = pf_utoa_base(buffer, value, base, upcase);
	put_sv(repr, out);
}

static void	put_unsigned_simple_padded(unsigned long long value, int min_width,
			enum e_align align, t_stream *out)
{
	char				buffer[MAXBUF_UTOA];
	t_sv				repr;
	int					base = 10;
	int					upcase = 0;

	repr = pf_utoa_base(buffer, value, base, upcase);
	put_sv_padded(repr, min_width, align, out);
}

static void	put_unsigned_simple_with_leading_zeros(unsigned long long value, int min_width,
			t_stream *out)
{
	char				buffer[MAXBUF_UTOA];
	t_sv				repr;
	int					base = 10;
	int					upcase = 0;

	repr = pf_utoa_base(buffer, value, base, upcase);
	while(min_width > repr.length) {
		pf_putc('0', out);
		min_width--;
	}
	put_sv(repr, out);
}

static void put_char(char c, t_stream *out)
{
	pf_putc(c, out);
}

static void	put_time(const time_t *timep, t_stream *out)
{
	char *month_string[12] = {"Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul",
		"Aug", "Sep", "Oct", "Nov", "Dec"};
	struct tm time_breakdown;
	localtime_r(timep, &time_breakdown);

	time_t now = time(NULL);
#define SIX_MONTHS_AS_SECONDS 15778476
	//if older than six months print year, otherwise hours:minutes
	if (now < *timep || now - *timep > SIX_MONTHS_AS_SECONDS)
	{
		//mmm
		put_sv((t_sv){month_string[time_breakdown.tm_mon], 3}, out);
		pf_putc(' ', out);
		//dd
		put_unsigned_simple_padded(time_breakdown.tm_mday, 2, Align_right, out);
		pf_putc(' ', out);
		//yyyy
		put_unsigned_simple_padded(time_breakdown.tm_year + 1900, 5, Align_right, out);
	}
	else
	{
		//mmm
		put_sv((t_sv){month_string[time_breakdown.tm_mon], 3}, out);
		pf_putc(' ', out);
		//dd
		put_unsigned_simple_padded(time_breakdown.tm_mday, 2, Align_right, out);
		pf_putc(' ', out);
		//hh:mm
		put_unsigned_simple_with_leading_zeros(time_breakdown.tm_hour, 2, out);
		pf_putc(':', out);
		put_unsigned_simple_with_leading_zeros(time_breakdown.tm_hour, 2, out);
	}
}

static void	print_detailed_info(struct s_finfo	*f, struct s_width w, t_stream *out)
{
	mode_t	mode;
	char	perms[10];

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

	put_sv((t_sv){perms, 10}, out);
	pf_putc(' ', out);
	put_unsigned_simple_padded(f->status.st_nlink, w.nlink, Align_right, out);
	pf_putc(' ', out);
	put_sv_padded((t_sv){f->owner, ft_strlen(f->owner)}, w.owner, Align_left, out);
	pf_putc(' ', out);
	put_sv_padded((t_sv){f->group, ft_strlen(f->group)}, w.group, Align_left, out);
	pf_putc(' ', out);
	put_unsigned_simple_padded(f->status.st_size, w.size, Align_right, out);
	pf_putc(' ', out);
	put_time(&(f->status.st_mtime), out);
	pf_putc(' ', out);
	put_sv(f->name, out);
	if (S_ISLNK(mode)) {
		put_sv((t_sv){" -> ", 4}, out);
		put_sv(f->linkname, out);
	}
	pf_putc('\n', out);
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

void ht_insert(struct hashNode **table, int key, char *value)
{
	int hash = calc_hash(key);
	if (table[hash] == NULL)
	{
		struct hashNode *new_node = malloc(sizeof(struct hashNode));
		*new_node = (struct hashNode){key, value, NULL};
		table[hash] = new_node;
		return;
	}
	struct hashNode *node = table[hash];
	while (1)
	{
		if (node->key == key)
		{
			node->value = value;
			return;
		}
		if (node->next == NULL)
		{
			struct hashNode *new_node = malloc(sizeof(struct hashNode));
			*new_node = (struct hashNode){key, value, NULL};
			node->next = new_node;
			return;
		}
		node = node->next;
	}
}

char *ht_search(struct hashNode **table, int key)
{
	struct hashNode *node = table[calc_hash(key)];
	while (node)
	{
		if (node->key == key)
		{
			return node->value;
		}
		node = node->next;
	}
	return NULL;
}

static void add_owner_and_group(struct s_finfo *info)
{
	static struct hashNode *owners[TABLE_SIZE];
	static struct hashNode *groups[TABLE_SIZE];

	char *owner_name = ht_search(owners, info->status.st_uid);
	char *group_name = ht_search(groups, info->status.st_gid);

	if (!owner_name)
	{
		struct passwd *my_passwd = getpwuid(info->status.st_uid);
		if (!my_passwd)
		{
			panic(Fail_serious, "can't get owner's name: %s\n", strerror(errno));
		}
		owner_name = ft_strdup(my_passwd->pw_name);
		if (!owner_name)
		{
			panic(Fail_serious, "malloc failed: %s\n", strerror(errno));
		}
		ht_insert(owners, info->status.st_uid, owner_name);
	}
	if (!group_name)
	{
		struct group *my_group = getgrgid(info->status.st_gid);
		if (!my_group)
		{
			panic(Fail_serious, "can't get group's name: %s\n", strerror(errno));
		}
		group_name = ft_strdup(my_group->gr_name);
		if (!group_name)
		{
			panic(Fail_serious, "malloc failed: %s\n", strerror(errno));
		}
		ht_insert(groups, info->status.st_gid, group_name);
	}
	info->owner = owner_name;
	info->group = group_name;
}

static void update_detail_meta(struct s_meta *meta, struct s_finfo *info)
{
	meta->total_blocks += info->status.st_blocks;
	meta->w.nlink = ft_max(meta->w.nlink, n_digits(info->status.st_nlink));
	meta->w.size = ft_max(meta->w.size, n_digits(info->status.st_size));
	meta->w.owner = ft_max(meta->w.owner, ft_strlen(info->owner));
	meta->w.group = ft_max(meta->w.group, ft_strlen(info->group));
	meta->w.name = ft_max(meta->w.name, info->name.length);
}

bool	print_informations(struct s_finfo **items, int item_count,
		t_options options, struct s_width detail_meta_w, t_stream *out)
{
	// need to print a newline before the first dir announcement?
	bool	had_printed = false;

	// do printing
	if (options.detailed_mode)
	{
		int i = 0;
		while (i < item_count)
		{
			print_detailed_info(items[i], detail_meta_w, out);
			i++;
			had_printed = true;
		}
	}
	else if (options.single_column)
	{
		int i = 0;
		while (i < item_count)
		{
			put_sv(items[i]->name, out);
			pf_putc('\n', out);
			i++;
			had_printed = true;
		}
	}
	else
	{
		const char    *const sep_string = "  ";
		t_sv	separator = (t_sv){sep_string, ft_strlen(sep_string)};
		int		*column_widths = NULL;
		int		stride = 1;
		if (item_count > 1)
		{
			int	termwidth = get_termwidth();
			stride = columnize(&column_widths, items, item_count, separator.length, termwidth);
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
				{
					put_sv_padded(items[idx]->name, column_widths[col], Align_left, out);
					put_sv(separator, out);
				}
				else
				{
					put_sv(items[idx]->name, out);
					pf_putc('\n', out);
				}
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

void ft_memcopy(char *dest, const char *src, int size)
{
	for (int i = 0; i < size; ++i)
	{
		dest[i] = src[i];
	}
}

/*
t_arena make_arena(int capacity, int *success)
{
	t_arena arena = {0};

	arena.memory = malloc(capacity);
	arena.capacity = capacity;
	*success = (arena.memory != 0);
	return arena;
}
*/

void *arena_allocate_bytes(t_arena *arena, int size, int *success)
{
	if (arena->offset + size > arena->capacity)
	{
		*success = 0;
		return NULL;
	}
	void *start = arena->memory + arena->offset;
	arena->offset += size;
	*success = 1;

	if (arena->offset > arena->max_offset)
		arena->max_offset = arena->offset;

	return start;
}

t_sv arena_push_cstring_with_terminating_0(t_arena *arena, const char *cstring, int *success)
{
	int size = ft_strlen(cstring) + 1;
	if (arena->offset + size > arena->capacity) {
		*success = 0;
		return (t_sv){0};
	}
	ft_memcopy(arena->memory + arena->offset, cstring, size);
	t_sv view = (t_sv){
		.start = arena->memory + arena->offset,
		.length = size - 1
	};
	arena->offset += size;
	*success = 1;

	if (arena->offset > arena->max_offset)
		arena->max_offset = arena->offset;

	return view;
}

t_sv arena_push_bytes(t_arena *arena, const char *source, int size, int *success)
{
	if (arena->offset + size > arena->capacity) {
		*success = 0;
		return (t_sv){0};
	}
	ft_memcopy(arena->memory + arena->offset, source, size);
	t_sv view = (t_sv){
		.start = arena->memory + arena->offset,
		.length = size
	};
	arena->offset += size;
	*success = 1;

	if (arena->offset > arena->max_offset)
		arena->max_offset = arena->offset;

	return view;
}

int get_file_info(struct s_finfo *info, const char *filename, struct s_meta *detail_meta,
				int dirfd, t_options options, int statflags, t_arena *names_arena)
{
	int ok;

	ok = fstatat(dirfd, filename, &info->status, statflags);
	if (ok == -1)
	{
		ft_dprintf(STDERR, "%s: cannot access '%s': %s\n", g_program_name, filename, strerror(errno));
		g_had_minor_errors = true;
		return Fail_minor;
	}
	if (options.detailed_mode)
	{
		info->linkname = (t_sv){0};
		if (S_ISLNK(info->status.st_mode))
		{
			char buffer[256];
			int length = readlinkat(dirfd, filename, buffer, 256);
			int success;
			info->linkname = arena_push_bytes(names_arena, buffer, length, &success);
			if (!success)
			{
				ft_dprintf(STDERR, "'%s': cannot push link name: names arena out of memory capacity\n", filename);
				g_had_minor_errors = true;
				return Fail_minor;

			}
		}
		add_owner_and_group(info);
		update_detail_meta(detail_meta, info);
	}
	return Success;
}

void	list_initial_paths(const char **paths, int path_count, t_options options,
		t_arena *names_arena, t_arena *infos_arena, t_stream *out)
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
		int success;
		struct s_finfo	*new_info = arena_allocate_bytes(infos_arena, sizeof(*new_info), &success);
		new_info->name.start = paths[i]; //TODO
		new_info->name.length = ft_strlen(paths[i]);
		int ok;
		//in detailed mode print info about links themselves
		if (options.detailed_mode)
			ok = get_file_info(new_info, paths[i], &detail_meta, AT_FDCWD, options, AT_SYMLINK_NOFOLLOW, names_arena);
		else
			ok = get_file_info(new_info, paths[i], &detail_meta, AT_FDCWD, options, 0, names_arena);
		if (ok != Success)
		{
			g_had_minor_errors = true;
			i++;
			continue;
		}
		if (S_ISDIR(new_info->status.st_mode))
			dirs[dir_count++] = new_info;
		else
			nondirs[nondir_count++] = new_info;
		i++;
	}

	// sort
	qsort(nondirs, nondir_count, sizeof(struct s_finfo *), g_compare);
	qsort(dirs, dir_count, sizeof(struct s_finfo *), g_compare);


	// print
	bool had_printed = print_informations(nondirs, nondir_count, options, detail_meta.w, out);

	// list directories (maybe recursively)
	i = 0;
	while (i < dir_count)
	{
		// don't go into links
		if (S_ISLNK(dirs[i]->status.st_mode))
		{
			i++;
			continue;
		}
		bool should_announce = (options.recursive || path_count > 1);
		char current_path[PATH_MAX] = {};
		ft_strcpy(current_path, dirs[i]->name.start);
		list_directory(current_path, STARTING_DEPTH + 1, options, &had_printed,
				&should_announce, names_arena, infos_arena, out);
		i++;
	}
}

void	list_directory(char *current_path, int depth, t_options options,
		bool *had_printed, bool *should_announce,
		t_arena *names_arena, t_arena *infos_arena, t_stream *out)
{
	DIR				*current_dir;
	if (depth >= MAX_DEPTH)
	{
		panic(Fail_serious, "%s: can't list '%s': reached max directory depth.\n", g_program_name, current_path);
	}
	current_dir = opendir(current_path);
	if (!current_dir)
	{
		ft_dprintf(STDERR, "%s: cannot open directory '%s': %s\n", g_program_name, current_path, strerror(errno));
		g_had_minor_errors = true;
		return;
	}

	//remember starting offsets of arenas
	int names_arena_checkpoint = names_arena->offset;
	int infos_arena_checkpoint = infos_arena->offset;
	struct s_finfo	*infos[MAX_BREADTH];
	int				entry_count = 0;
	// read filenames
	entry_count = 0;
	struct dirent	*entry;
	while ((entry = readdir(current_dir)))
	{
		if (entry_count >= MAX_BREADTH)
		{
			panic(Fail_serious, "%s: can't list '%s': reached max number of entries per directory.\n", g_program_name, current_path);
		}
		//skip invisible files
		if (entry->d_name[0] == '.' && !options.show_hidden_files)
		{
			continue;
		}
		int success;
		infos[entry_count] = arena_allocate_bytes(infos_arena, sizeof(struct s_finfo), &success);
		if (!success)
		{
			ft_dprintf(STDERR, "'%s': can't list all entries: infos arena out of memory capacity\n", current_path);
			g_had_minor_errors = true;
			break;
		}
		infos[entry_count]->name = arena_push_cstring_with_terminating_0(names_arena, entry->d_name, &success);
		if (!success)
		{
			ft_dprintf(STDERR, "'%s': can't list all entries: names arena out of memory capacity\n", current_path);
			g_had_minor_errors = true;
			break;
		}
		infos[entry_count]->is_dir = (entry->d_type == DT_DIR);
		entry_count++;
	}

	struct s_meta	detail_aggregates = {0};
	if (options.detailed_mode || options.sort_by_mtime)
	{
		for (int i = 0; i < entry_count; ++i)
		{
			get_file_info(infos[i], infos[i]->name.start, &detail_aggregates,
					dirfd(current_dir), options, AT_SYMLINK_NOFOLLOW, names_arena);
		}
	}

	// sort
	qsort(infos, entry_count, sizeof(*infos), g_compare);

	// print
	if (*should_announce)
	{
		if (*had_printed)
		{
			put_char('\n', out);
		}
		put_sv((t_sv){current_path, ft_strlen(current_path)}, out);
		put_char('\n', out);
		*had_printed = true;
	}
	*should_announce = true;
	if (options.detailed_mode)
	{
		put_sv((t_sv){"total ", 6}, out);
		put_unsigned_simple(detail_aggregates.total_blocks / BLOCK_HACK, out);
		put_char('\n', out);
	}
	*had_printed |= print_informations(infos, entry_count, options, detail_aggregates.w, out);

	t_sv	dir_names[MAX_BREADTH];
	int		dir_names_count = 0;
	// remember dir filenames after sorting
	// so that we can release infos memory
	// before stepping into directories
	if (options.recursive)
	{
		for (int i = 0; i < entry_count; ++i)
		{
			if (infos[i]->is_dir
					&& !ft_strequ(infos[i]->name.start, ".")
					&& !ft_strequ(infos[i]->name.start, ".."))
			{
				dir_names[dir_names_count++] = infos[i]->name;
			}
		}
	}
	// rollback infos memory
	infos_arena->offset = infos_arena_checkpoint;

	// step into directories
	for (int i = 0; i < dir_names_count; ++i)
	{
		{
			path_push(current_path, dir_names[i].start);
			list_directory(current_path, depth + 1, options, had_printed,
					should_announce, names_arena, infos_arena, out);
			path_pop(current_path);
		}
	}
	closedir(current_dir);
	// rollback names memory
	names_arena->offset = names_arena_checkpoint;
}

static void	putc_impl_ls_custom(int c, t_stream *b)
{
	int		written;

	if (b->space_left == 0)
	{
		written = write(b->fd, b->data, b->size);
		if (written < 0)
			pf_error("write error\n");
		b->total_written += written;
		b->pos = 0;
		b->space_left = b->size;
	}
	b->data[b->pos] = c;
	b->pos++;
	b->space_left--;
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
	int names_cap = 1024*1024/8;
	int infos_cap = 1024*1024*2;
	void *memory = malloc(names_cap + infos_cap);
	if (!memory)
	{
		ft_dprintf(STDERR, "failed to allocate memory: %s", strerror(errno));
		exit(Fail_serious);
	}
	t_arena	names_arena = (t_arena){.memory = memory, .capacity = names_cap};
	t_arena infos_arena = (t_arena){.memory = memory + names_cap, .capacity = infos_cap};

	char		print_buffer[BUFFER_SIZE];
	t_stream	out = pf_stream_init(STDOUT, print_buffer, BUFFER_SIZE, putc_impl_ls_custom);
	list_initial_paths(paths, path_count, options, &names_arena, &infos_arena, &out);
	pf_stream_flush(&out);

	if (options.mem_usage)
	{
		ft_printf("max usage: names = %d/%d, infos = %d/%d\n",
				names_arena.max_offset, names_arena.capacity,
				infos_arena.max_offset, infos_arena.capacity);
		ft_printf("(sum = %.2fK)\n", (names_arena.max_offset + infos_arena.max_offset)/1024.);
	}
	if (g_had_minor_errors)
		return (Fail_minor);
	return (Success);
}
