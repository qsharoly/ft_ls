/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   big_mul.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: qsharoly <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/08/06 19:40:23 by qsharoly          #+#    #+#             */
/*   Updated: 2021/02/24 09:53:35 by debby            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"
#include "bignum.h"

void	add_with_carry_internal(t_big *res, int at_index, t_digit_tmp carry)
{
	t_digit_tmp	sum;

	while (carry)
	{
		if (at_index >= BIG_N_DIGITS)
		{
			ft_putstr_fd(2, "bignum overflow!\n");
			return ;
		}
		sum = res->val[at_index] + carry;
		res->val[at_index] = sum % BIG_BASE;
		carry = sum / BIG_BASE;
		at_index++;
	}
	if (at_index > res->used)
		res->used = at_index;
}

t_big	big_mul(t_big a, t_big b)
{
	t_big		res;
	t_digit_tmp	tmp;
	int			i;
	int			j;

	res = big_zero();
	j = 0;
	while (j < b.used)
	{
		i = 0;
		while (i < a.used)
		{
			tmp = a.val[i] * b.val[j];
			add_with_carry_internal(&res, i + j, tmp);
			i++;
		}
		j++;
	}
	return (res);
}
