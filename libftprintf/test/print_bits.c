#include <stdio.h>

void	print_bits(void *a, int n_bits)
{
	int		i;

	i = n_bits - 1;
	while (i >= 0)
	{
		printf("%d", (*((unsigned long *)a) & (1L << i)) != 0);
		i--;
	}
}

void	print_bits2(void *a, int n_bits)
{
	int		i;
	int		bit;
	char	buf[1005] = {0};

	i = 0;
	while (i < n_bits)
	{
		bit = (*((unsigned long *)a + i / 64) & (1L << (i % 64))) != 0;
		buf[sizeof(buf) - i - 1] = '0' + bit;
		i++;
	}
	printf("%s", buf + sizeof(buf) - i);
}

void	print_double_bits(double a)
{
	printf("(0b");
	print_bits(&a, 64);
	printf(")\n");
}

void	print_ld_bits(long double a)
{
	printf("(0b");
	print_bits2(&a, 80);
	printf(")\n");
}

void	print_char_bits(char c)
{
	print_bits(&c, 8);
	printf("\n");
}
