/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pf_parse.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: qsharoly <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/04/14 13:26:37 by qsharoly          #+#    #+#             */
/*   Updated: 2021/02/28 14:00:56 by debby            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"
#include "libftprintf.h"

static int			is_printf_flag(int c)
{
	return (c == '0' || c == '-' || c == '+' || c == ' ' || c == '#');
}

static const char	*parse_flags(const char *pos, t_fmt *fmt)
{
	while (is_printf_flag(*pos))
	{
		fmt->pad_with_zero |= (*pos == '0');
		fmt->left_align |= (*pos == '-');
		fmt->explicit_plus |= (*pos == '+');
		fmt->prepend_space |= (*pos == ' ');
		fmt->alternative_form |= (*pos == '#');
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
		fmt->size = Size_longdouble;
	else
		fmt->size = Size_normal;
	if (fmt->size == Size_hh || fmt->size == Size_ll)
		pos += 2;
	else if (fmt->size == Size_h || fmt->size == Size_l
			|| fmt->size == Size_longdouble)
		pos += 1;
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

static char			choose_padchar(const t_fmt *fmt)
{
	char	padchar;

	if (fmt->pad_with_zero && !fmt->left_align)
		padchar = '0';
	else
		padchar = ' ';
	if (fmt->pad_with_zero && fmt->has_precision
		&& (fmt->write_arg == conv_signed || fmt->write_arg == conv_unsigned))
		padchar = ' ';
	if (fmt->write_arg == conv_str)
		padchar = ' ';
	return (padchar);
}

static const char	*parse_min_width(const char *pos, t_fmt *fmt, va_list ap)
{
	int		nb;

	if (*pos == '*')
	{
		pos++;
		nb = va_arg(ap, int);
		if (nb < 0)
			fmt->left_align = 1;
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

extern void			(*g_conv_table[256])(t_stream *, t_fmt *, va_list);

void				init_conv_table(void)
{
	ft_bzero(g_conv_table, sizeof(void *) * 256);
	g_conv_table['%'] = conv_percent;
	g_conv_table['c'] = conv_char;
	g_conv_table['s'] = conv_str;
	g_conv_table['p'] = conv_ptr;
	g_conv_table['d'] = conv_signed;
	g_conv_table['i'] = conv_signed;
	g_conv_table['u'] = conv_unsigned;
	g_conv_table['o'] = conv_unsigned;
	g_conv_table['x'] = conv_unsigned;
	g_conv_table['X'] = conv_unsigned;
	g_conv_table['f'] = conv_floating;
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
	fmt.padchar = choose_padchar(&fmt);
	fmt.spec_length = pos - str;
	return (fmt);
}
