/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   big_raise.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: qsharoly <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/08/06 19:45:05 by qsharoly          #+#    #+#             */
/*   Updated: 2023/10/18 16:25:31 by kith             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "bignum.h"

void	big_shallow_swap(t_big *a, t_big *b)
{
	t_big	tmp;

	tmp = *a;
	*a = *b;
	*b = tmp;
}

void	big_mul_by_small(t_big *res, t_digit b)
{
	t_twodigit carry = 0;
	int i = 0;
	while (i < res->used)
	{
		t_twodigit prod = (t_twodigit)res->val[i] * (t_twodigit)b + carry;
		res->val[i] = prod % BIG_BASE;
		carry = prod / BIG_BASE;
		i++;
	}
	big_addc_small(res, i, carry);
}

void	big_raise(t_big *res, t_digit base, t_digit power)
{
	if (base == 0 && power == 0)
	{
		big_set_to_small(res, 1);
		return;
	}
	if (base == 0)
	{
		big_set_to_small(res, 0);
		return;
	}
	big_set_to_small(res, 1);
	t_digit i = 0;
	while (i < power)
	{
		big_mul_by_small(res, base);
		i++;
	}
}
