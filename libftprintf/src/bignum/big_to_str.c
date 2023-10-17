/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   big_to_str.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: qsharoly <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/05/12 04:10:05 by qsharoly          #+#    #+#             */
/*   Updated: 2022/04/06 17:46:06 by debby            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libftprintf.h"
#include "bignum.h"

static char	*utoa_digit(char *pos, t_digit value, int must_fill)
{
	int	count;

	count = 0;
	while (value)
	{
		*pos = '0' + value % 10;
		value /= 10;
		pos--;
		count++;
	}
	if (must_fill)
	{
		while (count < BIG_CHARS_PER_DIGIT)
		{
			*pos = '0';
			pos--;
			count++;
		}
	}
	return (pos);
}

char	*big_str(char buf[BIG_MAX_CHARS + 1], t_big a)
{
	char	*pos;
	int		i;
	int		must_fill;

	if (a.overflow_occured)
		return ("bignum overflow!");
	pos = &buf[BIG_MAX_CHARS];
	*pos = '\0';
	pos--;
	i = 0;
	while (i < a.used)
	{
		must_fill = (i < a.used - 1);
		pos = utoa_digit(pos, a.val[i], must_fill);
		i++;
	}
	pos++;
	return (pos);
}
