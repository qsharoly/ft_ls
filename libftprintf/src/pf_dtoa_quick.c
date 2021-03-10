/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pf_dtoa_quick.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: qsharoly <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/05/12 04:49:33 by qsharoly          #+#    #+#             */
/*   Updated: 2021/02/28 12:06:35 by debby            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"
#include "libftprintf.h"
#include "float.h"
#include <limits.h>

static unsigned long	g_pow10[20] = {
	1L,
	10L,
	100L,
	1000L,
	10000L,
	100000L,
	1000000L,
	10000000L,
	100000000L,
	1000000000L,
	10000000000L,
	100000000000L,
	1000000000000L,
	10000000000000L,
	100000000000000L,
	1000000000000000L,
	10000000000000000L,
	100000000000000000L,
	1000000000000000000L,
	10000000000000000000UL
};

/*
** (rem == 0.5) tiebreak strategy: round half to even
*/

static void				part_and_round(t_parts *p, long double nb, int prec)
{
	long double	rem;

	p->ipart = ft_trunc(ft_fabs(nb));
	p->fpart = ((ft_fabs(nb) - p->ipart) * g_pow10[prec]);
	rem = p->fpart - ft_trunc(p->fpart);
	if (rem > 0.5)
		p->fpart++;
	else if (rem == 0.5 &&
		((unsigned long)ft_trunc(p->fpart) % 2 == 1
		|| (ft_trunc(p->fpart) == 0.0 && (unsigned long)p->ipart % 2 == 1)))
		p->fpart++;
	if (p->fpart > g_pow10[prec])
	{
		p->ipart++;
		p->fpart = 0;
	}
}

/*
** while (fpart * 10^i < 10^prec) i++;
*/

static int				calc_extra_zeros(long double fpart, int prec)
{
	int		i;

	i = 0;
	while (fpart * g_pow10[i] < g_pow10[prec] && i <= prec)
		i++;
	i = i - 1;
	return (i);
}

void					pf_dtoa_quick(t_stream *out, long double nb,
		const t_fmt *fmt)
{
	t_parts	p;
	char	buf[MAXBUF_UTOA * 2];
	int		pad_len;

	ft_bzero(&p, sizeof(p));
	p.sign = sign_char(ft_isneg(nb), fmt);
	p.dot = (fmt->precision > 0 || fmt->alternative_form) ? '.' : 0;
	part_and_round(&p, nb, fmt->precision);
	p.extra_zeros = calc_extra_zeros(p.fpart, fmt->precision);
	p.i_str = (p.ipart == 0.0) ? "0" : pf_utoa_base(buf, (unsigned long)p.ipart,
													10, 0);
	p.f_str = pf_utoa_base(buf + MAXBUF_UTOA, (unsigned long)p.fpart, 10, 0);
	pad_len = fmt->min_width - ((p.sign != 0) + ft_strlen(p.i_str)
			+ (p.dot != 0) + p.extra_zeros + ft_strlen(p.f_str));
	if (p.sign && fmt->pad_with_zero)
		pf_putc(p.sign, out);
	pf_repeat(fmt->padchar, !fmt->left_align * pad_len, out);
	if (p.sign && !fmt->pad_with_zero)
		pf_putc(p.sign, out);
	pf_puts(p.i_str, out);
	if (p.dot)
		pf_putc(p.dot, out);
	pf_repeat('0', p.extra_zeros, out);
	pf_puts(p.f_str, out);
	pf_repeat(fmt->padchar, fmt->left_align * pad_len, out);
}
