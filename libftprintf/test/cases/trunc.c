#include "../includes/float.h"
#include <stdio.h>
#include <math.h>
#include <limits.h>
#include <string.h>

int	check_double(double d)
{
	static int runs;
	double	actual = ft_trunc(d);
	double	expect = trunc(d);

	runs += 1;
	if (actual != expect)
	{
		printf("%d: failed at %f\n", runs, d);
		printf("ft_trunc -> %f\n   trunc -> %f\n",
				actual, expect);
	}
	return (actual == expect);
}

int	check_long_double(long double d)
{
	static int runs;
	long double	actual = ft_trunc(d);
	long double	expect = trunc(d);

	runs += 1;
	if (actual != expect)
	{
		printf("%d: failed at %Lf\n", runs, d);
		printf("ft_trunc -> %Lf\n   trunc -> %Lf\n",
				actual, expect);
	}
	return (actual == expect);
}

int	main(int ac, char **av)
{
	int runs;
	int ok_runs;
	double	d;
	long double	ld;
	union u_f80	subnormal;
	union u_f80	neg_subnormal;

	printf("\n=== Floats: trunc ===\n");
	printf("checking double:\n");
	runs = ok_runs = 0;
	d = (double)ULONG_MAX * 100;
	while (fabs(d *= -0.71) > 0.0001) { runs++; ok_runs += check_double(d); };
	printf("%d/%d OK\n", ok_runs, runs);
	printf("checking long double:\n");
	runs = ok_runs = 0;
	ld = (double)ULONG_MAX * 100;
	while (fabs(ld *= -0.71) > 0.0001) { runs++; ok_runs += check_long_double(ld); };
	printf("%d/%d OK\n", ok_runs, runs);
	printf("checking subnormals:\n");
	runs = ok_runs = 0;
	subnormal.bits = (struct s_f80_bits){ .sign = 0, .exponent = 0, .mantissa = 100500U };
	neg_subnormal.bits = (struct s_f80_bits){ .sign = 1, .exponent = 0, .mantissa = 421U };
	runs++; ok_runs += check_long_double(subnormal.f);
	runs++; ok_runs += check_long_double(neg_subnormal.f);
	printf("%d/%d OK\n", ok_runs, runs);
	printf("checking negative exponent:\n");
	runs = ok_runs = 0;
	runs++; ok_runs += check_long_double(0.0000000001);
	runs++; ok_runs += check_long_double(-0.0000000001);
	printf("%d/%d OK\n", ok_runs, runs);
	return (21);
}
