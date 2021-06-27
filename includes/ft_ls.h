/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_ls.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: debby <marvin@42.fr>                       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/03/10 07:50:12 by debby             #+#    #+#             */
/*   Updated: 2021/06/26 02:59:00 by debby            ###   ########.fr       */
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
#define LS_RECURSIVE (1<<4)
#define LS_SORT_BY_TIME (1<<5)
#define LS_TRANSPOSE_COLUMNS (1<<6)
#define LS_SINGLE_COLUMN (1<<7)

enum e_exitcode
{
	Success = 0,
	Fail_minor = 1,
	Fail_serious = 2,
};

struct		s_finfo
{
	struct stat		status;
	char			*name;
	char			*fullname;
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
