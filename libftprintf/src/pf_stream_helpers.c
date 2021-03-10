/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pf_stream_helpers.c                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: debby <marvin@42.fr>                       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/08/09 12:54:10 by debby             #+#    #+#             */
/*   Updated: 2021/02/28 11:05:19 by debby            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libftprintf.h"

void	pf_puts(const char *s, t_stream *b)
{
	while (*s)
	{
		pf_putc(*s, b);
		s++;
	}
}

void	pf_nputs(const char *s, int len, t_stream *b)
{
	while (*s && len-- > 0)
	{
		pf_putc(*s, b);
		s++;
	}
}

void	pf_repeat(char c, int times, t_stream *out)
{
	while (times-- > 0)
		pf_putc(c, out);
}
