/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_trunc.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: debby <marvin@42.fr>                       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/06/13 12:07:19 by debby             #+#    #+#             */
/*   Updated: 2020/08/21 21:01:40 by debby            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "float.h"

/*
** ft_trunc:
** +0.0, -0.0, +Inf, -Inf and NaN will be returned unchanged
*/

/*
**double		ft_trunc64(double d)
**{
**	union u_f64		tmp;
**	long			e;
**	unsigned long	m;
**
**	if (d == 0.0 || ft_isinf64(d) || ft_isnan64(d))
**		return (d);
**	if (ft_issub64(d))
**		return (d > 0.0 ? 0.0 : -0.0);
**	tmp.f = d;
**	e = tmp.bits.exponent - F64_BIAS;
**	if (e < 0)
**		return (d > 0.0 ? 0.0 : -0.0);
**	if (e > 52)
**		return (d);
**	m = tmp.bits.mantissa + (1L << 52);
**	tmp.bits.mantissa = (m >> (52 - e)) << (52 - e);
**	return (tmp.f);
**}
*/

/*
** TODO: needs thorough testing (mantissa has 64 bits, but somehow
** the function works correctly only when we shift by (63 - e), not (64 - e))...
*/

long double	ft_trunc(long double d)
{
	union u_f80		tmp;
	long			e;
	unsigned long	m;

	if (d == 0.0 || ft_isinf(d) || ft_isnan(d))
		return (d);
	if (ft_issub(d))
		return (d > 0.0 ? 0.0 : -0.0);
	tmp.f = d;
	e = tmp.bits.exponent - F80_BIAS;
	if (e < 0)
		return (d > 0.0 ? 0.0 : -0.0);
	if (e > 63)
		return (d);
	m = tmp.bits.mantissa;
	tmp.bits.mantissa = (m >> (63 - e)) << (63 - e);
	return (tmp.f);
}
