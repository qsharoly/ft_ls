/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pf_dtoa.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: qsharoly <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/05/12 04:49:33 by qsharoly          #+#    #+#             */
/*   Updated: 2023/10/17 12:26:47 by kith             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"
#include "libftprintf.h"
#include "bignum.h"
#include "float.h"

static void	digits_put(t_sv digits, int split_offset, t_sv sign, const t_fmt *fmt, t_stream *out)
{
	int		pad_len;
	int		leading_zeros;
	t_sv	dot;
	int		ilen;
	int		i;
	int		prec;

	dot = (fmt->precision > 0 || fmt->alternative_form) ? sv_from_cstr(".") : sv_from_cstr("");
	prec = fmt->precision;
	if (split_offset <= 0)
		ilen = 1; // single zero before the decimal point.
	else
		ilen = ft_min(digits.length, split_offset);
	pad_len = fmt->min_width - (sign.length + ilen + dot.length + prec);//ft_max(ft_min(digits.length - split_offset, prec), prec));
	leading_zeros = 0;
	if (fmt->align_right_by_leading_zeros && fmt->align == Align_right)
	{
		leading_zeros = pad_len;
		pad_len = 0;
	}
	put_repeat(' ', (fmt->align == Align_right) * pad_len, out);
	put_sv(sign, out);
	put_repeat('0', leading_zeros, out);
	if (split_offset <= 0)
	{
		pf_putc('0', out);
		put_sv(dot, out);
		while (split_offset++ < 0 && prec-- > 0)
			pf_putc('0', out);
		i = 0;
		while (i < digits.length && prec-- > 0)
			pf_putc(digits.start[i++], out);
		put_repeat('0', prec, out);
	}
	else
	{
		i = 0;
		while (i < ilen)
			pf_putc(digits.start[i++], out);
		put_sv(dot, out);
		while (i < digits.length && prec-- > 0)
			pf_putc(digits.start[i++], out);
		put_repeat('0', prec, out);
	}
	put_repeat(' ', (fmt->align == Align_left) * pad_len, out);
}

char	*digits_round(char *digits, int rounding_position)
{
	int	i;
	int	dig_len;
	int	rp;

	dig_len = ft_strlen(digits);
	rp = rounding_position;
	if (rp < 0 || rp > dig_len)
		return (digits);
	if (digits[rp] <= '4')
		return (digits);
	if (digits[rp] == '5')
	{
		//dont round up when precisely at a half (tail is all zeros)
		//and the digit before the rounding_position is even.
		i = rp + 1;
		while (i < dig_len && digits[i] == '0')
			i++;
		if (i == dig_len && (rp == 0 || (digits[rp - 1] - '0') % 2 == 0))
			return (digits);
	}
	//add +1 (with carry) to the digit before the rounding_position
	i = rp - 1;
	while (i >= 0 && digits[i] == '9')
	{
		digits[i] = '0';
		i--;
	}
	if (i < 0)
	{
		digits--;
		*digits = '1';
	}
	else
		digits[i] += 1;
	//zero out the tail
	i = rp + 1;
	while (i < dig_len)
	{
		digits[i] = '0';
		i++;
	}
	return (digits);
}

static long	get_exponent(long double nb)
{
	union u_f80	f;

	f.f = nb;
	return (f.bits.exponent);
}

static unsigned long	get_mantissa(long double nb)
{
	union u_f80	f;

	f.f = nb;
	return (f.bits.mantissa);
}

#ifdef DEBUGLOG
#include <stdio.h>
void	put_big(t_big a)
{
	printf("%*u ", BIG_CHARS_PER_DIGIT, a.val[a.used - 1]);
	for (int i = a.used - 2; i >= 0; i--)
	{
		printf("%0*u ", BIG_CHARS_PER_DIGIT, a.val[i]);
	}
	printf("\n");
}
#endif //DEBUGLOG

void	pf_dtoa(t_stream *out, long double nb, const t_fmt *fmt)
{
	long			exponent;
	unsigned long	mantissa;
	long			dec_pow;
	char			*digits;
	char			buf[BIG_MAX_CHARS + 1];

	exponent = get_exponent(nb);
	mantissa = get_mantissa(nb);
	exponent = exponent == 0 ? 1 - F80_BIAS : exponent - F80_BIAS;
	dec_pow = -63;
	while ((mantissa & 1L) == 0 && mantissa > 0)
	{
		mantissa >>= 1;
		dec_pow++;
	}
	if (exponent < 0)
	{
		dec_pow += exponent;
		exponent = 0;
	}
	if (mantissa == 0)
	{
		digits = "0";
	}
	else
	{
		t_digit	memory[3][BIG_N_DIGITS];
		t_big p, m, t;
		big_init(&p, memory[0], 0);
		big_init(&t, memory[1], 0);
		big_init(&m, memory[2], mantissa);


#ifdef DEBUGLOG
		printf("\n");
		printf("m: %lu\n", mantissa);
		printf("m used: %d\n", m.used); put_big(m);
		big_raise(&p, 5, -dec_pow);
		printf("5^%ld used: %d\n", -dec_pow, p.used); put_big(p);
		big_mul(&t, &m, &p);
		big_shallow_swap(&m, &t);
		printf("m * 5^%ld used: %d\n", -dec_pow, m.used); put_big(m);
		big_raise(&p, 2, exponent);
		printf("2^%ld used: %d\n", exponent, p.used); put_big(p);
		big_mul(&t, &m, &p);
		big_shallow_swap(&m, &t);
		printf("m * 5^%ld * 2^%ld used: %d\n", -dec_pow, exponent, m.used); put_big(m);
#else
		big_raise(&p, 5, -dec_pow);
		big_mul(&t, &m, &p);
		big_shallow_swap(&m, &t);
		big_raise(&p, 2, exponent);
		big_mul(&t, &m, &p);
		big_shallow_swap(&m, &t);
#endif //DEBUGLOG
		if (m.overflow_occured)
		{
			put_sv(sv_from_cstr("bignum overflow!"), out);
			return ;
		}
		digits = big_str(buf, m);
		digits = digits_round(digits, ft_strlen(digits) + dec_pow + fmt->precision);
	}
	if (digits)
		digits_put((t_sv){ digits, ft_strlen(digits) }, ft_strlen(digits) + dec_pow,
				sign_prefix(fp_isneg(nb), fmt), fmt, out);
}
