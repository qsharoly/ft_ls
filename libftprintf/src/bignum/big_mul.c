/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   big_mul.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: qsharoly <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/08/06 19:40:23 by qsharoly          #+#    #+#             */
/*   Updated: 2022/04/08 10:24:46 by debby            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"
#include "bignum.h"

void	ensure_zero_init_if_past_used(t_big *res, int at_index)
{
	if (at_index >= BIG_N_DIGITS)
	{
		res->overflow_occured = 1;
		return ;
	}
	if (at_index >= res->used)
	{
		res->used++;
		res->val[at_index] = 0;
	}
}

void	big_addc_small(t_big *res, int at_index, t_twodigit carry)
{
	t_twodigit	sum;

	if (res->overflow_occured)
		return ;
	while (carry)
	{
		ensure_zero_init_if_past_used(res, at_index);
		sum = res->val[at_index] + carry;
		res->val[at_index] = sum % BIG_BASE;
		carry = sum / BIG_BASE;
		at_index++;
	}
}

void	big_mul(t_big *res, const t_big *a, const t_big *b)
{
	t_twodigit	tmp;
	int			i;
	int			j;

	if (a->overflow_occured || b->overflow_occured)
	{
		res->overflow_occured = 1;
		return ;
	}
	big_set_to_small(res, 0);
	j = 0;
	while (j < b->used)
	{
		i = 0;
		while (i < a->used)
		{
			tmp = (t_twodigit)a->val[i] * (t_twodigit)b->val[j];
			ensure_zero_init_if_past_used(res, i + j);
			big_addc_small(res, i + j, tmp);
			if (res->overflow_occured)
				return ;
			i++;
		}
		j++;
	}
}
