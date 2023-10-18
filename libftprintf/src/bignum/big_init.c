/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   big_init.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: qsharoly <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/08/06 19:33:10 by qsharoly          #+#    #+#             */
/*   Updated: 2023/10/18 16:25:50 by kith             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "bignum.h"

void	big_set_to_small(t_big *a, t_twodigit n)
{
	a->used = 1;
	a->val[0] = n % BIG_BASE;
	while (n /= BIG_BASE)
	{
		a->val[a->used] = n % BIG_BASE;
		a->used++;
	}
}

void	big_init(t_big *a, t_digit mem[BIG_N_DIGITS], t_twodigit n0)
{
	a->val = mem;
	big_set_to_small(a, n0);
	a->overflow_occured = 0;
}
