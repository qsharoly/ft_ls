/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   big_from_digit.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: qsharoly <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/08/06 19:50:10 by qsharoly          #+#    #+#             */
/*   Updated: 2021/02/23 15:15:32 by debby            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "bignum.h"

t_big	big_from_number(unsigned long nb)
{
	t_big	big;
	int		i;

	big = big_zero();
	i = 0;
	while (nb)
	{
		big.val[i] = nb % BIG_BASE;
		nb /= BIG_BASE;
		big.used++;
		i++;
	}
	return (big);
}
