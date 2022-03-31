/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   fp_issub.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: debby <marvin@42.fr>                       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/06/18 15:11:29 by debby             #+#    #+#             */
/*   Updated: 2022/03/27 20:42:18 by debby            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "float.h"

/*
**int		fp_issub64(double d)
**{
**	union u_f64	tmp;
**
**	tmp.f = d;
**	if (tmp.bits.exponent == 0)
**		return (1);
**	else
**		return (0);
**}
*/

int	fp_issub(long double d)
{
	union u_f80	tmp;

	tmp.f = d;
	if (tmp.bits.exponent == 0)
		return (1);
	else
		return (0);
}
