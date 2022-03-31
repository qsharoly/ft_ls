/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   big_add.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: qsharoly <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/08/06 19:35:29 by qsharoly          #+#    #+#             */
/*   Updated: 2022/03/31 15:13:47 by debby            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "bignum.h"

t_big	big_add(t_big a, t_big b)
{
	t_big		res;
	t_digit_tmp	tmp;
	int			i;
	int			used;

	res = big_zero();
	used = a.used > b.used ? a.used : b.used;
	i = 0;
	while (i < used)
	{
		tmp = a.val[i] + b.val[i];
		add_with_carry_internal(&res, i, tmp);
		if (res.overflow_occured)
			return (res);
		i++;
	}
	return (res);
}
