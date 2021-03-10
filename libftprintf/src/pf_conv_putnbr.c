/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pf_conv_putnbr.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: qsharoly <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/08/06 19:10:22 by qsharoly          #+#    #+#             */
/*   Updated: 2021/02/28 12:06:20 by debby            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"
#include "libftprintf.h"

void	pf_putnbr(t_stream *out, const char *value_start, const char *prefix,
			const t_fmt *fmt)
{
	int				value_len;
	int				pre_len;
	int				zero_fill;
	int				pad_len;

	value_len = ft_strlen(value_start);
	pre_len = ft_strlen(prefix);
	if (fmt->pad_with_zero && !fmt->left_align && !fmt->has_precision)
	{
		zero_fill = ft_max(0, fmt->min_width - value_len - pre_len);
		pad_len = 0;
	}
	else
	{
		zero_fill = ft_max(0, fmt->precision - value_len);
		pad_len = fmt->min_width - pre_len - zero_fill - value_len;
	}
	pf_repeat(fmt->padchar, !fmt->left_align * pad_len, out);
	if (pre_len)
		pf_puts(prefix, out);
	pf_repeat('0', zero_fill, out);
	pf_puts(value_start, out);
	pf_repeat(fmt->padchar, fmt->left_align * pad_len, out);
}
