/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pf_utoa_base.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: qsharoly <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/05/17 08:37:39 by qsharoly          #+#    #+#             */
/*   Updated: 2022/03/23 20:59:39 by debby            ###   ########.fr       */
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
				unsigned int base, const char *digits)
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

t_sv	pf_utoa_base(char *buffer, unsigned long long value,
				unsigned int base, int upcase)
{
	t_sv		view;
	char		*pos;
	const char	*digits;

	digits = upcase ? "0123456789ABCDEF" : "0123456789abcdef";
	pos = &buffer[MAXBUF_UTOA - 1];
	if (base == 16)
		pos = fast_utoa_hex(pos, value, digits);
	else if (base == 8)
		pos = fast_utoa_oct(pos, value, digits);
	else
		pos = general_utoa_base(pos, value, base, digits);
	view.start = pos;
	view.length = MAXBUF_UTOA - (pos - buffer);
	return (view);
}
