/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   big_init.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: qsharoly <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/08/06 19:33:10 by qsharoly          #+#    #+#             */
/*   Updated: 2022/04/08 07:30:46 by debby            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "bignum.h"

void	big_init(t_big *a, t_digit mem[BIG_N_DIGITS], t_twodigit n0)
{
	a->val = mem;
	big_set_to_small(a, n0);
	a->overflow_occured = 0;
}
