/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pf_conv_basic.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: qsharoly <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/04/14 12:23:11 by qsharoly          #+#    #+#             */
/*   Updated: 2021/03/10 06:20:52 by debby            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stdlib.h>
#include "libftprintf.h"
#include "libft.h"

#if __APPLE__

void	conv_percent(t_stream *out, t_fmt *f, va_list ap)
{
	int		pad_len;

	(void)ap;
	pad_len = ft_max(0, f->min_width - 1);
	pf_repeat(f->padchar, !f->left_align * pad_len, out);
	pf_putc('%', out);
	pf_repeat(f->padchar, f->left_align * pad_len, out);
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
	char	c;

	c = (char)va_arg(ap, int);
	pad_len = ft_max(0, f->min_width - 1);
	pf_repeat(f->padchar, !f->left_align * pad_len, out);
	pf_putc(c, out);
	pf_repeat(f->padchar, f->left_align * pad_len, out);
}

void	conv_str(t_stream *out, t_fmt *f, va_list ap)
{
	int		value_len;
	int		pad_len;
	char	*s;

	s = va_arg(ap, char *);
	if (!s)
	{
		if (f->has_precision && f->precision < 6)
			s = "";
		else
			s = "(null)";
	}
	if (f->has_precision && f->precision >= 0)
		value_len = ft_min(ft_strlen(s), f->precision);
	else
		value_len = ft_strlen(s);
	pad_len = ft_max(0, f->min_width - value_len);
	pf_repeat(f->padchar, !f->left_align * pad_len, out);
	pf_nputs(s, value_len, out);
	pf_repeat(f->padchar, f->left_align * pad_len, out);
}
