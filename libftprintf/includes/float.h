/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   float.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: debby <marvin@42.fr>                       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/06/13 11:52:21 by debby             #+#    #+#             */
/*   Updated: 2022/03/27 20:38:10 by debby            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef FLOAT_H
# define FLOAT_H

# define F64_BIAS 1023
# define F80_BIAS 16383

union					u_f64
{
	double				f;
	struct {
		unsigned long	mantissa:52;
		unsigned int	exponent:11;
		unsigned int	sign:1;
	}					bits;
};

union					u_f80
{
	long double			f;
	struct s_f80_bits {
		unsigned long	mantissa:64;
		unsigned int	exponent:15;
		unsigned int	sign:1;
	}					bits;
};

int						fp_isneg(long double d);
int						fp_isinf(long double d);
int						fp_isnan(long double d);
int						fp_issub(long double d);
long double				fp_fabs(long double d);
long double				fp_trunc(long double d);

#endif
