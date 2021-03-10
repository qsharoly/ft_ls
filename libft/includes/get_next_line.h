/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   get_next_line.h                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: qsharoly <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/09/12 11:56:19 by qsharoly          #+#    #+#             */
/*   Updated: 2019/09/20 19:17:15 by qsharoly         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef GET_NEXT_LINE_H
# define GET_NEXT_LINE_H

# include <unistd.h>
# ifdef __linux__
	#include <stdint.h>
# endif

# define BUFF_SIZE 1024

typedef struct	s_buf
{
	int		fd;
	char	data[BUFF_SIZE];
	char	*delim_pos;
	ssize_t	len;
}				t_buf;

int				get_next_line(const int fd, char **line);

#endif
