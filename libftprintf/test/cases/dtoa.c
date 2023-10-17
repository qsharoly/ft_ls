#include "libftprintf.h"
#include <stdio.h>
#include <float.h>
#include <strings.h>
#include "../print_bits.c"
#include <limits.h>

double	bits2double(char *bit_values);
void	check_dbl(const char *format, double a, const char *literal);
void	check_ld(const char *format, long double a, const char *literal);

int		main(void)
{
	ft_printf("\n=== dtoa correctness ===\n");
	//*
	//specials
	check_dbl("%f", 0.0 / 0.0, "0.0 / 0.0");
	check_dbl("%f", -0.0 / 0.0, "-0.0 / 0.0");
	check_dbl("%+f", 0.0 / 0.0, "0.0 / 0.0");
	print_double_bits(0.0 / 0.0);
	print_double_bits(-0.0 / 0.0);
	print_ld_bits(0.0 / 0.0);
	print_ld_bits(-0.0 / 0.0);
	check_dbl("%f", 1.0 / 0.0, "1.0 / 0.0");
	check_dbl("%f", -1.0 / 0.0, "-1.0 / 0.0");
	check_dbl("%+f", 1.0 / 0.0, "1.0 / 0.0");
	print_double_bits(1.0 / 0.0);
	print_double_bits(-1.0 / 0.0);
	print_ld_bits(1.0 / 0.0);
	print_ld_bits(-1.0 / 0.0);
	//*/
	//*
	//basic
	check_dbl("%f", 8000, "8000");
	check_dbl("%.1f", 7.5, "7.5");
	check_dbl("%.2f", 7.5, "7.5");
	check_dbl("%.3f", 7.5, "7.5");
	check_dbl("%.4f", 7.5, "7.5");
	check_dbl("%.5f", 7.5, "7.5");
	check_dbl("%f", 0.0, "0.0");
	check_dbl("%f", -958.125, "-958.125");
	check_dbl("%f", 1.05, "1.05");
	check_dbl("%.4f", 1.05, "1.05");
	check_dbl("%.4f", 1.532673, "1.532673");
	check_dbl("%f", 0.000039, "0.000039");
	check_dbl("%.0f", 0.000039, "0.000039");
	check_dbl("%5.1f", 7.3, "7.3");
	check_dbl("%5.1f", -7.3, "-7.3");
	//*/
	check_dbl("%-5.0f", -7.3, "-7.3");
	check_dbl("%-5.0f", 7.5, "7.5");
	check_dbl("%-5.0f", -7.5, "-7.5");
	check_dbl("%-5.3f", 0.0, "0.0");
	check_dbl("%f", -3.85, "-3.85");
	//*
	check_dbl("%f", -0.99999949, "-0.99999949");
	check_dbl("%f", 1444565444646.6465424242242, "1444565444646.6465424242242");
	check_dbl("%lf", 1444565444646.6465424242242, "1444565444646.6465424242242");
	check_ld("%Lf", 1444565444646.6465424242242l, "1444565444646.6465424242242l");
	check_ld("%Lf", -958.125, "-958.125");
	check_ld("%Lf", -0.3, "-0.3");
	//*/
	//*
	//zeros in front
	check_dbl("%010f", 0.5, "0.5");
	//zeros after
	check_dbl("%.10f", 0.5, "0.5");
	//space in front (slow path)
	check_dbl("%30.25f", 0.5, "0.5");
	//zeros in front (slow path)
	check_dbl("%0105.100f", 0.237, "0.237");
	//zeros in middle (slow path)
	check_dbl("%0105.100f", 0.00000237, "0.00000237");
	//debug bignum used digit count tracking
	check_dbl("%.100f", 0.237, "0.237");
	//*/
	//*
	//rounding
	check_dbl("%f", -56.2012685, "-56.2012685");
	check_dbl("%f", 56.2012685, "56.2012685");
	check_dbl("%f", -56.2012675, "-56.2012675");
	check_dbl("%f", 56.2012675, "56.2012675");
	check_ld("%Lf", -56.2012685l, "-56.2012685l");
	check_ld("%Lf", 56.2012685l, "56.2012685l");
	check_ld("%Lf", -56.2012675l, "-56.2012675l");
	check_ld("%Lf", 56.2012675l, "56.2012675l");
	check_dbl("%f", 0.0894255, "0.0894255");
	check_dbl("%f", 43.4399999, "43.4399999");
	check_dbl("%f", -5.0299999, "-5.0299999");
	//*/
	//*
	//leak into whole part
	check_dbl("%f", 3.9999999, "3.9999999");
	check_dbl("%f", -5.9999999, "-5.9999999");
	//*/
	//*
	//round half to even
	check_dbl("%.0f", 1.5, "1.5");
	check_dbl("%.0f", 2.5, "2.5");
	check_dbl("%.0f", -5.5, "-5.5");
	check_dbl("%.0f", -2.5, "-2.5");
	check_dbl("%.0f", -0.5, "-0.5");
	//big numbers and limits
	check_dbl("%.2000f", __DBL_MIN__, "DBL_MIN");
	check_dbl("%f", __DBL_MAX__, "DBL_MAX");
	check_ld("%.20000Lf", __LDBL_MIN__, "LDBL_MIN");
	check_ld("%Lf", __LDBL_MAX__, "LDBL_MAX");
	check_ld("%Lf", 12345678901234567890123456789012345678901.125, "12345678901234567890123456789012345678901.125");
	print_ld_bits(12345678901234567890123456789012345678901.125);
	print_ld_bits(__LDBL_MAX__);
	print_ld_bits(__LDBL_MIN__);
	//*/
	return (0);
}

double	bits2double(char *bit_values)
{
	int		i;
	long	nb;

	nb = 0;
	i = 0;
	while (i < 64)
	{
		nb += (1 << i) * (bit_values[64 - i] == '1');
		i++;
	}
	return (*((double *)&(nb)));
}

void	check_dbl(const char *format, double a, const char *literal)
{
	printf("checking (\"%s\", %s)\n", format, literal);
	printf("expected: \"");
	printf(format, a);
	printf("\"\n  actual: \"");
	fflush(stdout);
	ft_printf(format, a);
	printf("\"\n\n");
}

void	check_ld(const char *format, long double a, const char *literal)
{
	printf("checking (\"%s\", %s)\n", format, literal);
	printf("expected: \"");
	printf(format, a);
	printf("\"\n  actual: \"");
	fflush(stdout);
	ft_printf(format, a);
	printf("\"\n\n");
}
