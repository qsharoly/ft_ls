/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pf_utoa_base.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: qsharoly <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/05/17 08:37:39 by qsharoly          #+#    #+#             */
/*   Updated: 2020/08/09 15:33:56 by debby            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libftprintf.h"

static char	*fast_utoa_hex(char *pos, unsigned long long value,
				const char *digits)
{
	while (value)
	{
		*pos = digits[value & 0xf];
		value >>= 4;
		pos--;
	}
	pos++;
	return (pos);
}

static char	*fast_utoa_oct(char *pos, unsigned long long value,
				const char *digits)
{
	while (value)
	{
		*pos = digits[value & 7];
		value >>= 3;
		pos--;
	}
	pos++;
	return (pos);
}

static char	*general_utoa_base(char *pos, unsigned long long value,
				unsigned base, const char *digits)
{
	while (value)
	{
		*pos = digits[value % base];
		value /= base;
		pos--;
	}
	pos++;
	return (pos);
}

char		*pf_utoa_base(char *buffer, unsigned long long value,
				unsigned base, int upcase)
{
	char		*pos;
	const char	*digits;

	digits = upcase ? "0123456789ABCDEF" : "0123456789abcdef";
	pos = &buffer[MAXBUF_UTOA - 1];
	*pos = '\0';
	pos--;
	if (base == 16)
		return (fast_utoa_hex(pos, value, digits));
	else if (base == 8)
		return (fast_utoa_oct(pos, value, digits));
	return (general_utoa_base(pos, value, base, digits));
}
