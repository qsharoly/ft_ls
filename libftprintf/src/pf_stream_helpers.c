/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pf_stream_helpers.c                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: debby <marvin@42.fr>                       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/08/09 12:54:10 by debby             #+#    #+#             */
/*   Updated: 2022/03/23 20:19:37 by debby            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libftprintf.h"

void	put_repeat(char c, int times, t_stream *out)
{
	while (times-- > 0)
		pf_putc(c, out);
}

void	put_sv(t_sv view, t_stream *b)
{
	while (view.length-- > 0)
		pf_putc(*(view.start++), b);
}

void	put_sv_padded(t_sv view, int pad_len, enum e_align align, t_stream *b)
{
	if (align == AlignLeft)
	{
		put_sv(view, b);
		put_repeat(' ', pad_len, b);
	}
	else if (align == AlignRight)
	{
		put_repeat(' ', pad_len, b);
		put_sv(view, b);
	}
	else
	{
		// bad arguments
	}
}

