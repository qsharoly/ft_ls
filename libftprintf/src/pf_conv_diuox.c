/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pf_conv_diuox.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: qsharoly <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/04/14 12:55:31 by qsharoly          #+#    #+#             */
/*   Updated: 2021/02/28 11:50:00 by debby            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"
#include "libftprintf.h"

#if __linux__

void	conv_ptr(t_stream *out, t_fmt *fmt, va_list ap)
{
	char			str[MAXBUF_UTOA];
	const char		*value_start;
	void			*p;

	p = va_arg(ap, void *);
	if (!p)
	{
		pf_putnbr(out, "(nil)", "", fmt);
		return ;
	}
	value_start = pf_utoa_base(str, (unsigned long)p, 16, 0);
	pf_putnbr(out, value_start, "0x", fmt);
}

#else

void	conv_ptr(t_stream *out, t_fmt *fmt, va_list ap)
{
	char			str[MAXBUF_UTOA];
	const char		*value_start;
	void			*p;

	p = va_arg(ap, void *);
	value_start = pf_utoa_base(str, (unsigned long)p, 16, 0);
	pf_putnbr(out, value_start, "0x", fmt);
}

#endif

void	conv_signed(t_stream *out, t_fmt *fmt, va_list ap)
{
	char			str[MAXBUF_UTOA];
	const char		*value_start;
	char			prefix[2];
	long long int	arg;

	if (fmt->size == Size_ll)
		arg = va_arg(ap, long long);
	else if (fmt->size == Size_l)
		arg = va_arg(ap, long);
	else
		arg = va_arg(ap, int);
	if (fmt->size == Size_hh)
		arg = (char)arg;
	else if (fmt->size == Size_h)
		arg = (short)arg;
	value_start = pf_utoa_base(str, ft_abs(arg), 10, 0);
	prefix[0] = sign_char(arg < 0, fmt);
	prefix[1] = '\0';
	pf_putnbr(out, value_start, prefix, fmt);
}

void	conv_unsigned(t_stream *out, t_fmt *fmt, va_list ap)
{
	char				str[MAXBUF_UTOA];
	const char			*value_start;
	char				*prefix;
	unsigned long long	arg;

	if (fmt->size == Size_ll)
		arg = va_arg(ap, unsigned long long);
	else if (fmt->size == Size_l)
		arg = va_arg(ap, unsigned long);
	else
		arg = va_arg(ap, unsigned int);
	if (fmt->size == Size_hh)
		arg = (unsigned char)arg;
	else if (fmt->size == Size_h)
		arg = (unsigned short)arg;
	value_start = pf_utoa_base(str, arg, fmt->base, fmt->upcase);
	prefix = "";
	if (fmt->alternative_form)
	{
		if (fmt->base == 8 && ((int)ft_strlen(value_start) >= fmt->precision))
			prefix = "0";
		if (fmt->base == 16 && arg > 0)
			prefix = fmt->upcase ? "0X" : "0x";
	}
	pf_putnbr(out, value_start, prefix, fmt);
}
