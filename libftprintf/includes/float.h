/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   float.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: debby <marvin@42.fr>                       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/06/13 11:52:21 by debby             #+#    #+#             */
/*   Updated: 2020/08/21 20:59:55 by debby            ###   ########.fr       */
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
	struct {
		unsigned long	mantissa:64;
		unsigned int	exponent:15;
		unsigned int	sign:1;
	}					bits;
};

int						ft_isneg(long double d);
int						ft_isinf64(double d);
int						ft_isinf(long double d);
int						ft_isnan64(double d);
int						ft_isnan(long double d);
int						ft_issub64(double d);
int						ft_issub(long double d);
double					ft_fabs64(double d);
long double				ft_fabs(long double d);
double					ft_trunc64(double d);
long double				ft_trunc(long double d);

#endif
