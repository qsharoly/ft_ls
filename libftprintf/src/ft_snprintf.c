/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_snprintf.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: qsharoly <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/09/27 18:24:37 by qsharoly          #+#    #+#             */
/*   Updated: 2022/10/25 04:01:03 by debby            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <unistd.h>
#include "libftprintf.h"

/*
** if `out->size == 0` we still go through the conversion
** to count how many chars would be written
** (`putc_impl_snprintf` will do nothing but increase `out->total_written`
** in this case)
*/

static void	print_args(t_stream *out, const char *format, va_list ap)
{
	t_fmt	fmt;

	while ((out->space_left > 0 || (out->size == 0)) && *format)
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

/*
** if no more space left, but we are in the middle of a variable,
** `putc_impl_snprintf` will do a no-op until the variable finishes.
** after that, printing will terminate in `print_args` function.
**
** `total_written` is always incremented to count how many characters
** would be written when `bufsz` is set to 0.
*/

static void	putc_impl_snprintf(int c, t_stream *b)
{
	b->total_written++;
	// leave at least one byte for terminating null
	if (b->space_left <= 1)
		return ;
	if (b->data)
	{
		b->data[b->pos] = c;
		b->pos++;
		b->space_left--;
	}
}

// https://en.cppreference.com/w/c/io/fprintf
//
// 4) Writes the results to a character string `buffer`.
// At most `bufsz - 1` characters are written. The resulting character string
// will be terminated with a null character, unless `bufsz` is zero.
// If `bufsz` is zero, nothing is written and `buffer` may be a null pointer,
// however the return value (number of bytes that would be written not including
// the null terminator) is still calculated and returned.
__attribute__((__format__(printf, 3, 4)))
int			ft_snprintf(char *buffer, int bufsz, const char *format, ...)
{
	va_list		ap;
	t_stream	b;

	b = pf_stream_init(STDOUT, buffer, bufsz, putc_impl_snprintf);
	va_start(ap, format);
	print_args(&b, format, ap);
	va_end(ap);
	if (buffer && bufsz > 0)
	{
		buffer[b.pos] = '\0';
	}
	return (b.total_written);
}
