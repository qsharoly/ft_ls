/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_ls.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: debby <marvin@42.fr>                       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/03/10 07:50:12 by debby             #+#    #+#             */
/*   Updated: 2023/02/11 02:33:51 by kith             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef FT_LS_H
# define FT_LS_H

#include <stddef.h>
#include <sys/stat.h>
#include <stdbool.h>
#include "sv.h"

#define STARTING_DEPTH 0
#define MAX_DEPTH 25
#define MAX_BREADTH 10000

#define BLOCK_HACK 2

typedef struct s_options	t_options;
struct	s_options
{
	bool	show_hidden_files;
	bool	detailed_mode;
	bool	recursive;
	bool	reverse_sort;
	bool	sort_by_mtime;
	bool	single_column;
	bool	mem_usage;
};

enum e_exitcode
{
	Success = 0,
	Fail_minor = 1,
	Fail_serious = 2,
};

typedef struct
{
	int		capacity;
	int		offset;
	int		max_offset;
	void	*memory;
} t_arena;

struct	s_finfo
{
	t_sv		name;
	struct stat	status;
	char		*owner;
	char		*group;
	t_sv		linkname;
	bool		is_dir;
};

struct s_width {
	int	nlink;
	int	size;
	int	name;
	int	owner;
	int	group;
};

struct	s_meta
{
	size_t			total_blocks;
	struct s_width	w;
};

int		columnize(int **column_widths, struct s_finfo **items, int item_count,
			int separator_width, int width_limit);

#endif
