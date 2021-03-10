/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   big_to_str.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: qsharoly <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/05/12 04:10:05 by qsharoly          #+#    #+#             */
/*   Updated: 2021/03/10 08:57:07 by debby            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"
#include "libftprintf.h"
#include "bignum.h"

static char	*utoa_digit(char *pos, t_digit value, int fill_at_least)
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
	while (count < fill_at_least)
	{
		*pos = '0';
		pos--;
		count++;
	}
	return (pos);
}

char		*big_str(char buf[BIG_MAX_CHARS + 1], t_big a)
{
	char	*pos;
	int		i;
	int		fill_at_least;


	if (a.overflow_occured)
	{
		ft_strcpy(buf, "bignum overflow!");
		return (buf);
	}
	pos = &buf[BIG_MAX_CHARS];
	*pos = '\0';
	pos--;
	i = 0;
	while (i < a.used)
	{
		fill_at_least = i < a.used - 1 ? BIG_DIGIT_CHARS : 0;
		pos = utoa_digit(pos, a.val[i], fill_at_least);
		i++;
	}
	pos++;
	return (pos);
}
