/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pf_conv_putnbr.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: qsharoly <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/08/06 19:10:22 by qsharoly          #+#    #+#             */
/*   Updated: 2022/03/25 22:49:07 by debby            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"
#include "libftprintf.h"

void	pf_putnbr(t_stream *out, t_sv value, t_sv prefix, const t_fmt *fmt)
{
	int				zero_fill;
	int				pad_length;

	if (fmt->align_right_by_leading_zeros && fmt->align == AlignRight && !fmt->has_precision)
	{
		zero_fill = ft_max(0, fmt->min_width - value.length - prefix.length);
		pad_length = 0;
	}
	else
	{
		zero_fill = ft_max(0, fmt->precision - value.length);
		pad_length = fmt->min_width - prefix.length - zero_fill - value.length;
	}
	put_repeat(' ', (fmt->align == AlignRight) * pad_length, out);
	put_sv(prefix, out);
	put_repeat('0', zero_fill, out);
	put_sv(value, out);
	put_repeat(' ', (fmt->align == AlignLeft) * pad_length, out);
}
