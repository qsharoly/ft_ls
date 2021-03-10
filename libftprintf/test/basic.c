#include "libftprintf.h"
#include <stdio.h>
#include <limits.h>

int		main(void)
{
	int		i;
	char	*s;

	s = "hello";
	//while (1)
	{
		//simple cases
		/*
		ft_printf("");
		ft_printf("just a string\n");
		ft_printf("just an int %d def\n", 5);
		ft_printf("%s well\n", "very");
		ft_printf("my %s equals %d%%\n", s, -123);
		ft_printf("abc %x %X %x %X\n", 42, 42, -42, -42);
		printf("abc %x %X %x %X\n", 42, 42, -42, -42);
		*/
		//percent sign
		ft_printf("percent sign%%ef\n");
		//+ without specifier
		ft_printf("%");
		//+ with precision and width
		ft_printf("lama");
		ft_printf("%5");
		ft_printf("goo");
		ft_printf("%5");
		ft_printf("\n");
		//+ without specifier
		printf("%");
		//+ with precision and width
		printf("lama");
		printf("%5");
		printf("goo");
		printf("%5");
		printf("\n");
		/*
		//alternative form
		ft_printf("Lower hex %#x\n", 243);
		ft_printf("Upper hex %#X\n", 255);
		*/
		//+ octal zero
		ft_printf("%#o %#.o %#.0o\n", 0, 0, 0);
		printf("%#o %#.o %#.0o\n", 0, 0, 0);
		//+ octal with precision
		ft_printf("%#.3o %#o %#.1o\n", 1, 1, 1);
		printf("%#.3o %#o %#.1o\n", 1, 1, 1);
		//+ octal with width
		ft_printf("%#6o\n", 3);
		printf("%#6o\n", 3);
		ft_printf("%#-6o\n", 3);
		printf("%#-6o\n", 3);
		/*
		//+ hex precision
		ft_printf("%#.3x %#04x %#04.3x %#04.5x %#06.4x %#06.5x\n", 0x123, 0x123, 0x123, 0x123, 0x123, 0x123);
		printf("%#.3x %#04x %#04.3x %#04.5x %#06.4x %#06.5x\n", 0x123, 0x123, 0x123, 0x123, 0x123, 0x123);
		*/
		/*
		//conversions
		ft_printf("%d\n", INT_MIN);
		ft_printf("%hhu\n", -1);
		ft_printf("%hu\n", -1);
		ft_printf("%u\n", -1);
		ft_printf("%lu\n", -1);
		ft_printf("%llu\n", ULLONG_MAX);
		   printf("%hhu\n", -1);
		   printf("%hu\n", -1);
		   printf("%u\n", -1);
		   printf("%lu\n", -1);
		   printf("%llu\n", -1);
		   printf("%llu\n", ULLONG_MAX);
		//min width and padding flags
		ft_printf("%10d\n", 12);
		printf("%10d\n", 12);
		ft_printf("%010d\n", 10);
		printf("%010d\n", 10);
		ft_printf("%010d\n", -10);
		printf("%010d\n", -10);
		ft_printf("% 10d\n", 33);
		ft_printf("%-10dtons\n", 100);
		ft_printf("%-010dignorewhat\n", 23);
		//+ precision
		*/
		ft_printf("%.5d\n", 34);
		printf("%.5d\n", 34);
		ft_printf("%10.5d\n", 10);
		printf("%10.5d\n", 10);
		ft_printf("%-10.5d\n", 10);
		printf("%-10.5d\n", 10);
		/*
		ft_printf("% 0+8.5d\n", 34);
		printf("% 0+8.5d\n", 34);
		ft_printf("% 0+8.3d\n", -8473);
		printf("% 0+8.3d\n", -8473);
		ft_printf("%06.3i\n", 0);
		printf("%06.3i\n", 0);
		ft_printf("%06.3i\n", 1);
		printf("%06.3i\n", 1);
		ft_printf("%06.d %06.0d\n", 1, 2);
		printf("%06.d %06.0d\n", 1, 2);
		//+ strings
		ft_printf("%10s\n %010s\n", s, s);
		printf("%10s\n %010s\n", s, s);
		ft_printf("%10.s\n%10.0s\n%10.1s\n", s, s, s);
		printf("%10.s\n%10.0s\n%10.1s\n", s, s, s);
		//+ left-justify
		ft_printf("%-10.s\n%-10.0s\n%-10.1s\n", s, s, s);
		printf("%-10.s\n%-10.0s\n%-10.1s\n", s, s, s);
		*/
		//pointers
		ft_printf("%p %p %p, %.2p\n", s, NULL, printf, NULL);
		printf("%p %p %p, %.2p\n", s, NULL, printf, NULL);
		ft_printf("%20p %-20p\n", NULL, NULL);
		printf("%20p %-20p\n", NULL, NULL);
		/*
		*/
		/*
		//negative cases
		ft_printf("no type % 0ratata %d\n", 13, 14);
		printf("no type % 0ratata %d\n", 15, 16);
		ft_printf("type mi%smatch\n", 100500);//segfault
		ft_printf("what if %d\n", "i put a string there");//print some integer
		*/
	}
	return (0);
}
