/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_fabs.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: debby <marvin@42.fr>                       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/06/13 09:38:53 by debby             #+#    #+#             */
/*   Updated: 2020/08/21 21:03:34 by debby            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "float.h"

/*
**double		ft_fabs64(double nb)
**{
**	union u_f64	tmp;
**
**	tmp.f = nb;
**	tmp.bits.sign = 0;
**	return (tmp.f);
**}
*/

long double	ft_fabs(long double nb)
{
	union u_f80	tmp;

	tmp.f = nb;
	tmp.bits.sign = 0;
	return (tmp.f);
}
