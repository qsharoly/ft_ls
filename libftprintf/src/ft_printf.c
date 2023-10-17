/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_printf.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: qsharoly <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/09/27 18:24:37 by qsharoly          #+#    #+#             */
/*   Updated: 2023/10/17 11:26:02 by kith             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <unistd.h>
#include <stdlib.h>
#include "libft.h"
#include "libftprintf.h"

void			pf_error(const char *msg)
{
	write(2, msg, ft_strlen(msg));
	exit(1);
}

static void		print_args(t_stream *out, const char *format, va_list ap)
{
	t_fmt			fmt;

	while (*format)
	{
		if (*format == '%')
		{
			fmt = pf_parse_specifier(format, ap);
			if (fmt.write_arg)
				fmt.write_arg(out, &fmt, ap);
			format += fmt.spec_length;
			continue ;
		}
		pf_putc(*format, out);
		format++;
	}
}

static void	putc_impl_printf(int c, t_stream *b)
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

__attribute__((__format__(__printf__, 1, 2)))
int				ft_printf(const char *format, ...)
{
	va_list		ap;
	char		buffer[BUFFER_SIZE];
	t_stream	b;

	b = pf_stream_init(STDOUT, buffer, BUFFER_SIZE, putc_impl_printf);
	va_start(ap, format);
	print_args(&b, format, ap);
	va_end(ap);
	pf_stream_flush(&b);
	return (b.total_written);
}

__attribute__((__format__(__printf__, 2, 3)))
int				ft_dprintf(int fd, const char *format, ...)
{
	va_list		ap;
	t_stream	b;
	char		buffer[BUFFER_SIZE];

	b = pf_stream_init(fd, buffer, BUFFER_SIZE, putc_impl_printf);
	va_start(ap, format);
	print_args(&b, format, ap);
	va_end(ap);
	pf_stream_flush(&b);
	return (b.total_written);
}

int				ft_vdprintf(int fd, const char *format, va_list ap)
{
	t_stream	b;
	char		buffer[BUFFER_SIZE];

	b = pf_stream_init(fd, buffer, BUFFER_SIZE, putc_impl_printf);
	print_args(&b, format, ap);
	pf_stream_flush(&b);
	return (b.total_written);
}
