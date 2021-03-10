#include "../includes/float.h"
#include <stdio.h>
#include <math.h>
#include <limits.h>
#include <string.h>

void	check_trunc(double d)
{
	double	actual = ft_trunc(d);
	double	expect = trunc(d);

	if (actual != expect)
	{
		printf("failed at %f:\n", d);
		printf("ft_trunc -> %f\n   trunc -> %f\n",
				actual, expect);
	}
}

void	check_truncl(long double d)
{
	long double	actual = ft_truncl(d);
	long double	expect = trunc(d);

	if (actual != expect)
	{
		printf("failed at %Lf:\n", d);
		printf("ft_trunc -> %Lf\n   trunc -> %Lf\n",
				actual, expect);
	}
}

int		main(int ac, char **av)
{
	if (ac > 1)
	{
		if (strcmp(av[1], "perf1") == 0)
		{
			double	d = (double)ULONG_MAX;
			while (fabs(d *= -0.71) > 0.0001)
				printf("%f\n", ft_trunc(d));
		}
		else if (strcmp(av[1], "perflib") == 0)
		{
			double	d = (double)ULONG_MAX;
			while (fabs(d *= -0.71) > 0.0001)
				printf("%f\n", trunc(d));
		}
		return (42);
	}
	double	d = (double)ULONG_MAX * 100;
	while (fabs(d *= -0.71) > 0.0001)
		check_trunc(d);
	long double	ld = (double)ULONG_MAX * 100;
	while (fabs(ld *= -0.71) > 0.0001)
		check_trunc(ld);
	return (21);
}

