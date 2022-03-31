/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pf_conv_basic.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: qsharoly <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/04/14 12:23:11 by qsharoly          #+#    #+#             */
/*   Updated: 2022/03/24 01:16:16 by debby            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stdlib.h>
#include "libftprintf.h"
#include "libft.h"

#if __APPLE__

void	conv_percent(t_stream *out, t_fmt *f, va_list ap)
{
	int		pad_len;
	char	cto[1];

	(void)ap;
	cto[0] = '%';
	pad_len = f->min_width - 1;
	put_sv_padded((t_sv){ cto, 1 }, pad_len, f->align, out);
}

#elif __linux__

void	conv_percent(t_stream *out, t_fmt *f, va_list ap)
{
	(void)ap;
	(void)f;
	pf_putc('%', out);
}

#endif

void	conv_char(t_stream *out, t_fmt *f, va_list ap)
{
	int		pad_len;
	char	c[1];

	c[0] = (char)va_arg(ap, int);
	pad_len = f->min_width - 1;
	put_sv_padded((t_sv){ c, 1 }, pad_len, f->align, out);
}

void	conv_str(t_stream *out, t_fmt *f, va_list ap)
{
	int		pad_len;
	char	*s;
	t_sv	view;

	s = va_arg(ap, char *);
	if (!s)
	{
		if (f->has_precision && f->precision < 6)
			s = "";
		else
			s = "(null)";
	}
	view.start = s;
	if (f->has_precision && f->precision >= 0)
		view.length = ft_min(ft_strlen(s), f->precision);
	else
		view.length = ft_strlen(s);
	pad_len = f->min_width - view.length;
	put_sv_padded(view, pad_len, f->align, out);
}
