/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pf_stream_helpers.c                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: debby <marvin@42.fr>                       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/08/09 12:54:10 by debby             #+#    #+#             */
/*   Updated: 2022/10/22 14:03:35 by debby            ###   ########.fr       */
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

void	put_sv_padded_len(t_sv view, int pad_len, enum e_align align, t_stream *b)
{
	if (align == Align_left)
	{
		put_sv(view, b);
		put_repeat(' ', pad_len, b);
	}
	else if (align == Align_right)
	{
		put_repeat(' ', pad_len, b);
		put_sv(view, b);
	}
	else
	{
		// bad arguments
	}
}

void	put_sv_padded(t_sv view, int min_width, enum e_align align, t_stream *b)
{
	int pad_len = min_width - view.length;
	if (align == Align_left)
	{
		put_sv(view, b);
		put_repeat(' ', pad_len, b);
	}
	else if (align == Align_right)
	{
		put_repeat(' ', pad_len, b);
		put_sv(view, b);
	}
	else
	{
		// bad arguments
	}
}
