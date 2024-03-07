/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pf_parse.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: qsharoly <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/04/14 13:26:37 by qsharoly          #+#    #+#             */
/*   Updated: 2024/02/17 08:51:59 by kith             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"
#include "libftprintf.h"

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

static const char	*parse_min_width(const char *pos, t_fmt *fmt, va_list *ap)
{
	int		nb;

	if (*pos == '*')
	{
		pos++;
		nb = va_arg(*ap, int);
		if (nb < 0)
			fmt->align = Align_left;
		fmt->min_width = ft_abs(nb);
	}
	else
		pos = parse_a_number(pos, &fmt->min_width);
	return (pos);
}

static const char	*parse_precision(const char *pos, t_fmt *fmt, va_list *ap)
{
	int		prec;

	if (*pos == '.')
	{
		pos++;
		if (*pos == '*')
		{
			pos++;
			prec = va_arg(*ap, int);
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

static const char	*parse_base_and_case(const char *pos, t_fmt *f)
{
	f->base = 10 * (*pos == 'd' || *pos == 'i' || *pos == 'u')
		+ 8 * (*pos == 'o')
		+ 16 * (*pos == 'x' || *pos == 'X');
	f->upcase = (*pos == 'X');
	return (pos);
}

size_t	pf_parse_specifier(t_fmt *fmt, const char *str, va_list *ap)
{
	const char	*pos;

	ft_bzero((char *)fmt, sizeof(t_fmt));
	pos = str + 1; //skip initial `%`
	pos = parse_flags(pos, fmt);
	pos = parse_min_width(pos, fmt, ap);
	pos = parse_precision(pos, fmt, ap);
	pos = parse_size_modifier(pos, fmt);
	pos = parse_base_and_case(pos, fmt);
	fmt->type_char = *pos;
	if (ft_strchr("%diuoxXcspfv", fmt->type_char)==NULL)
		return (0);
	pos++;
	if (!fmt->has_precision)
	{
		if (ft_strchr("f", fmt->type_char))
			fmt->precision = 6;
		else
			fmt->precision = 1;
	}
	return (pos - str);
}

void	write_argument(t_stream *out, const t_fmt *fmt, va_list *ap)
{
	switch (fmt->type_char) {
	case '%':
		conv_percent(out, fmt, ap);
		break;
	case 'd':
	case 'i':
		conv_signed(out, fmt, ap);
		break;
	case 'u':
	case 'o':
	case 'x':
	case 'X':
		conv_unsigned(out, fmt, ap);
		break;
	case 'c':
		conv_character(out, fmt, ap);
		break;
	case 's':
		conv_cstr(out, fmt, ap);
		break;
	case 'p':
		conv_pointer(out, fmt, ap);
		break;
	case 'f':
		conv_floating(out, fmt, ap);
		break;
	case 'v':
		conv_sv(out, fmt, ap);
		break;
	default:
		;
	}
}
