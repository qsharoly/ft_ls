/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   fp_isnan.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: debby <marvin@42.fr>                       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/06/18 15:10:55 by debby             #+#    #+#             */
/*   Updated: 2022/03/27 20:41:42 by debby            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "float.h"

/*
**int		fp_isnan64(double nb)
**{
**	union u_f64	tmp;
**
**	tmp.f = nb;
**	if (tmp.bits.exponent == 0x7ff && tmp.bits.mantissa != 0)
**		return (tmp.bits.sign ? -1 : 1);
**	else
**		return (0);
**}
*/

int	fp_isnan(long double nb)
{
	union u_f80	tmp;

	tmp.f = nb;
	if (tmp.bits.exponent == 0x7fff && (tmp.bits.mantissa << 1) != 0)
	{
		if (tmp.bits.sign)
			return (-1);
		else
			return (1);
	}
	else
		return (0);
}
