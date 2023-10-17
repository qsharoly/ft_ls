/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pf_stream.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: qsharoly <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/08/06 19:26:20 by qsharoly          #+#    #+#             */
/*   Updated: 2022/04/10 08:08:05 by debby            ###   ########.fr       */
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
	b.used = 0;
	b.fd = fd;
	b.total_written = 0;
	return (b);
}

/*
** if data buffer is not empty, write to output
*/

void		pf_stream_flush(t_stream *b)
{
	int		written;

	if (b->used > 0)
	{
		written = write(b->fd, b->data, b->used);
		if (written < 0)
			pf_error("write error\n");
		b->total_written += written;
		b->used = 0;
	}
}

void		pf_putc(int c, t_stream *out)
{
	out->putc(c, out);
}
