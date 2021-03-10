/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   big_raise.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: qsharoly <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/08/06 19:45:05 by qsharoly          #+#    #+#             */
/*   Updated: 2021/02/18 18:37:28 by debby            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "bignum.h"

t_big	big_raise(t_digit small_base, t_digit power)
{
	t_big	res;
	t_big	base;

	if (small_base == 0 && power == 0)
		return (big_from_number(1));
	if (small_base == 0)
		return (big_from_number(0));
	res = big_from_number(1);
	base = big_from_number(small_base);
	while (power-- > 0)
		res = big_mul(res, base);
	return (res);
}
