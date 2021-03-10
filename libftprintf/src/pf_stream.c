/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pf_stream.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: qsharoly <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/08/06 19:26:20 by qsharoly          #+#    #+#             */
/*   Updated: 2021/03/10 07:34:19 by debby            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <unistd.h>
#include "libftprintf.h"

t_stream	pf_stream_init(int fd, char *buffer, int size,
				void (*putc)(int, t_stream *))
{
	t_stream b;

	b.putc = putc;
	b.data = buffer;
	b.size = size;
	b.fd = fd;
	b.total_written = 0;
	b.space_left = size;
	b.pos = 0;
	return (b);
}

/*
** if data buffer is not empty, write to output
*/

void		pf_stream_flush(t_stream *b)
{
	int		written;

	if (b->space_left != b->size)
	{
		written = write(b->fd, b->data, b->size - b->space_left);
		if (written < 0)
			pf_error("write error\n");
		b->total_written += written;
		b->pos = 0;
		b->space_left = b->size;
	}
}

void		pf_putc(int c, t_stream *out)
{
	out->putc(c, out);
}
