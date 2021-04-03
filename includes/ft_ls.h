/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_ls.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: debby <marvin@42.fr>                       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/03/10 07:50:12 by debby             #+#    #+#             */
/*   Updated: 2021/04/03 04:03:15 by debby            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef FT_LS_H
# define FT_LS_H

#include <sys/stat.h>

#define STARTING_DEPTH 0
#define MAX_DEPTH 25
#define MAX_WIDTH 1000

#define BLOCK_HACK 2

#define HAS_FILENAMES 1
#define LS_LIST_ALL (1<<1)
#define LS_VERBOSE (1<<2)
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

struct		s_finfo
{
	struct stat		status;
	char			*fullpath;
	char			*name;
	char			*owner;
	char			*group;
};

struct		s_col_widths
{
	int				lnk;
	int				size;
	int				name;
	int				owner;
	int				group;
};

#endif
