#include "libftprintf.h"
#include <stdio.h>

void	check_d(const char *format, int param, int value, const char *literals)
{
	printf("checking (\"%s\", %s)\n", format, literals);
	printf("expected: \"");
	printf(format, param, value);
	printf("\"\n  actual: \"");
	fflush(stdout);
	ft_printf(format, param, value);
	printf("\"\n\n");
}

int		main(void)
{
	//wildcard width & precision
	check_d("{%*3d}", 5, 0, "5, 0");
	check_d("{%05.*d}", -15, 42, "-15, 42");
	return (0);
}
