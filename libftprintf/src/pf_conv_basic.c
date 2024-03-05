/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pf_conv_basic.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: qsharoly <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/04/14 12:23:11 by qsharoly          #+#    #+#             */
/*   Updated: 2023/10/17 12:44:10 by kith             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stdlib.h>
#include "libftprintf.h"
#include "libft.h"

#if __APPLE__

void	conv_percent(t_stream *out, const t_fmt *f, va_list *ap)
{
	char	cto[1];

	(void)*ap;
	cto[0] = '%';
	put_sv_padded((t_sv){ cto, 1 }, f->min_width, f->align, out);
}

#elif __linux__

void	conv_percent(t_stream *out, const t_fmt *f, va_list *ap)
{
	(void)ap;
	(void)f;
	pf_putc('%', out);
}

#endif

void	conv_character(t_stream *out, const t_fmt *f, va_list *ap)
{
	char	c[1];

	c[0] = (char)va_arg(*ap, int);
	put_sv_padded((t_sv){ c, 1 }, f->min_width, f->align, out);
}

void	conv_cstr(t_stream *out, const t_fmt *f, va_list *ap)
{
	char	*s;
	t_sv	view;

	s = va_arg(*ap, char *);
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
	put_sv_padded(view, f->min_width, f->align, out);
}

void	conv_sv(t_stream *out, const t_fmt *f, va_list *ap)
{
	t_sv	view;

	view = va_arg(*ap, t_sv);
	if (!view.start)
	{
		if (f->has_precision && f->precision < 6)
		{
			view = (t_sv){"", 0};
		}
		else
		{
			view = (t_sv){"(null)", 6};
		}
	}
	if (f->has_precision && f->precision >= 0)
		view.length = ft_min(view.length, f->precision);
	put_sv_padded(view, f->min_width, f->align, out);
}
