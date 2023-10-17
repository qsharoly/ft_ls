/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   bignum.h                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: qsharoly <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/05/12 04:19:11 by qsharoly          #+#    #+#             */
/*   Updated: 2022/04/08 11:09:41 by debby            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef BIGNUM_H
# define BIGNUM_H

/* enough for worst case: all 11451 decimals of __LDBL_MIN__ :) */

# define BIG_N_DIGITS 1432
# define BIG_BASE 100000000
# define BIG_CHARS_PER_DIGIT 8
# define BIG_MAX_CHARS (BIG_CHARS_PER_DIGIT * BIG_N_DIGITS)

/*
** `t_digit` must be enough to hold (BIG_BASE)
** `t_twodigit` must be enough to hold (BIG_BASE * BIG_BASE)
**
*/

typedef unsigned int  t_digit;
typedef unsigned long t_twodigit;

typedef	struct			s_big
{
	t_digit		*val;
	short int	used;
	short int	overflow_occured:1;
}						t_big;

void	big_init(t_big *a, t_digit mem[BIG_N_DIGITS], t_twodigit n0);
void	big_set_to_small(t_big *a, t_twodigit n);
void	big_shallow_swap(t_big *a, t_big *b);
void	big_addc_small(t_big *res, int at_index, t_twodigit n);
void	big_mul(t_big *res, const t_big *a, const t_big *b);
void	big_raise(t_big *res, t_digit base, t_digit power);
char	*big_str(char buf[BIG_MAX_CHARS + 1], t_big a);

#endif
