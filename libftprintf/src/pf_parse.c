/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pf_parse.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: qsharoly <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/04/14 13:26:37 by qsharoly          #+#    #+#             */
/*   Updated: 2022/10/22 15:44:35 by debby            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"
#include "libftprintf.h"

static void	(* const g_conv_table[256])(t_stream *, t_fmt *, va_list) = {
	['%'] = conv_percent,
	['c'] = conv_char,
	['s'] = conv_cstr,
	['p'] = conv_ptr,
	['d'] = conv_signed,
	['i'] = conv_signed,
	['u'] = conv_unsigned,
	['o'] = conv_unsigned,
	['x'] = conv_unsigned,
	['X'] = conv_unsigned,
	['f'] = conv_floating,
	['v'] = conv_sv,
};

static int			is_flag_character(char c)
{
	return (c == '0' || c == '-' || c == '+' || c == ' ' || c == '#');
}

static const char	*parse_flags(const char *pos, t_fmt *fmt)
{
	while (is_flag_character(*pos))
	{
		if (*pos == '0')
			fmt->align_right_by_leading_zeros = 1;
		else if (*pos == '-')
			fmt->align = Align_left;
		else if (*pos == '+')
			fmt->plus_mode = ExplicitPlus;
		else if (*pos == ' ' && fmt->plus_mode != ExplicitPlus)
			fmt->plus_mode = ExplicitSpace;
		else if (*pos == '#')
			fmt->alternative_form = 1;
		pos++;
	}
	return (pos);
}

static const char	*parse_size_modifier(const char *pos, t_fmt *fmt)
{
	if (*pos == 'h' && *(pos + 1) == 'h')
		fmt->size = Size_hh;
	else if (*pos == 'h')
		fmt->size = Size_h;
	else if (*pos == 'l' && *(pos + 1) == 'l')
		fmt->size = Size_ll;
	else if (*pos == 'l')
		fmt->size = Size_l;
	else if (*pos == 'L')
		fmt->size = Size_L;
	if (fmt->size == Size_h || fmt->size == Size_l || fmt->size == Size_L)
		pos += 1;
	else if (fmt->size == Size_hh || fmt->size == Size_ll)
		pos += 2;
	return (pos);
}

static const char	*parse_a_number(const char *pos, int *value)
{
	*value = 0;
	while (*pos >= '0' && *pos <= '9')
	{
		*value = *value * 10 + (*pos - '0');
		pos++;
	}
	return (pos);
}

static const char	*parse_min_width(const char *pos, t_fmt *fmt, va_list ap)
{
	int		nb;

	if (*pos == '*')
	{
		pos++;
		nb = va_arg(ap, int);
		if (nb < 0)
			fmt->align = Align_left;
		fmt->min_width = ft_abs(nb);
	}
	else
		pos = parse_a_number(pos, &fmt->min_width);
	return (pos);
}

static const char	*parse_precision(const char *pos, t_fmt *fmt, va_list ap)
{
	int		prec;

	if (*pos == '.')
	{
		pos++;
		if (*pos == '*')
		{
			pos++;
			prec = va_arg(ap, int);
			if (prec < 0)
				return (pos);
		}
		else
			pos = parse_a_number(pos, &prec);
		fmt->precision = prec;
		fmt->has_precision = 1;
	}
	return (pos);
}

static const char	*parse_conv(const char *pos, t_fmt *f)
{
	if (ft_strchr("diuoxX", *pos))
	{
		f->upcase = (*pos == 'X');
		f->base = 10 * !!ft_strchr("diu", *pos) + 8 * (*pos == 'o') + 16 * !!ft_strchr("xX", *pos);
	}
	f->write_arg = g_conv_table[(unsigned char)*pos];
	if (f->write_arg)
		pos++;
	return (pos);
}

t_fmt				pf_parse_specifier(const char *str, va_list ap)
{
	t_fmt		fmt;
	const char	*pos;

	pos = str + 1;
	ft_bzero((char *)&fmt, sizeof(t_fmt));
	fmt.precision = 1;
	pos = parse_flags(pos, &fmt);
	pos = parse_min_width(pos, &fmt, ap);
	pos = parse_precision(pos, &fmt, ap);
	pos = parse_size_modifier(pos, &fmt);
	pos = parse_conv(pos, &fmt);
	fmt.spec_length = pos - str;
	return (fmt);
}
