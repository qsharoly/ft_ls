/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pf_conv_floating.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: qsharoly <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/05/12 05:31:25 by qsharoly          #+#    #+#             */
/*   Updated: 2021/02/28 12:06:25 by debby            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"
#include "libftprintf.h"
#include "float.h"
#include <limits.h>

char		sign_char(int is_negative, const t_fmt *fmt)
{
	if (is_negative)
		return ('-');
	else if (fmt->explicit_plus)
		return ('+');
	else if (fmt->prepend_space)
		return (' ');
	else
		return (0);
}

static int	put_if_special(t_stream *out, t_fmt *fmt, long double nb)
{
	char	*s;
	char	sign;
	int		pad_len;

	s = NULL;
	if (ft_isnan(nb))
		s = "nan";
	else if (ft_isinf(nb))
		s = "inf";
	if (s == NULL)
		return (0);
	sign = sign_char(ft_isneg(nb), fmt);
	pad_len = fmt->min_width - ((sign != '\0') + ft_strlen(s));
	pf_repeat(' ', !fmt->left_align * pad_len, out);
	if (sign)
		pf_putc(sign, out);
	pf_puts(s, out);
	pf_repeat(' ', fmt->left_align * pad_len, out);
	return (1);
}

void		conv_floating(t_stream *out, t_fmt *fmt, va_list ap)
{
	long double	d;

	if (fmt->size == Size_longdouble)
		d = va_arg(ap, long double);
	else
		d = va_arg(ap, double);
	if (fmt->has_precision == 0)
		fmt->precision = DTOA_DEFAULT_PRECISION;
	if (put_if_special(out, fmt, d))
		return ;
	if (ft_fabs(d) < (long double)ULONG_MAX && fmt->precision < 20)
		pf_dtoa_quick(out, d, fmt);
	else
		pf_dtoa(out, d, fmt);
}
