#include "libft.h"
#include "libftprintf.h"
#include <stdio.h>
#include <limits.h>

typedef int (*t_printf)(const char *, ...);

int		main(int argc, char **argv)
{
	int		i;
	int		reps = 10000;
	char	*st;
	t_printf print = ft_printf;
	if (argc == 2 && ft_strcmp(argv[1], "-libc") == 0) {
		print = printf;
	}

	st = "hello";
	print("\n=== Basic formatting ===\n");
	while (reps--)
	{
		//simple cases
		print("");
		print("just a string\n");
		print("just an int %d def\n", 5);
		print("%s well\n", "very");
		print("my %s equals %d%%\n", st, -123);
		print("abc %x %X %x %X\n", 42, 42, -42, -42);
		//percent sign
		print("percent sign%%df\n");
		//+ without specifier
		print("%");
		//+ with precision and width
		print("lama");
		print("%5");
		print("goo");
		print("%5");
		print("\n");
		/*
		//alternative form
		print("Lower hex %#x\n", 243);
		print("Upper hex %#X\n", 255);
		*/
		//+ octal zero
		print("%#o %#.o %#.0o\n", 0, 0, 0);
		//+ octal with precision
		print("%#.3o %#o %#.1o\n", 1, 1, 1);
		//+ octal with width
		print("%#6o\n", 3);
		print("%#-6o\n", 3);
		/**/
		//+ hex precision
		print("%#.3x %#04x %#04.3x %#04.5x %#06.4x %#06.5x\n", 0x123, 0x123, 0x123, 0x123, 0x123, 0x123);
		/**/
		/**/
		//conversions
		print("%d\n", INT_MIN);
		print("%hhu\n", -1);
		print("%hu\n", -1);
		print("%u\n", -1);
		print("%lu\n", -1);
		print("%llu\n", ULLONG_MAX);
		//min width and padding flags
		print("%10d\n", 12);
		print("%010d\n", 10);
		print("%010d\n", -10);
		print("% 10d\n", 33);
		print("%-10dtons\n", 100);
		print("%-010dignorewhat\n", 23);
		//+ precision
		/**/
		print("%.5d\n", 34);
		print("%10.5d\n", 10);
		print("%-10.5d\n", 10);
		/**/
		print("% 0+8.5d\n", 34);
		print("% 0+8.3d\n", -8473);
		print("%06.3i\n", 0);
		print("%06.3i\n", 1);
		print("%06.d %06.0d\n", 1, 2);
		//+ strings
		print("%10s\n %010s\n", st, st);
		print("%10.s\n%10.0s\n%10.1s\n", st, st, st);
		//+ left-justify
		print("%-10.s\n%-10.0s\n%-10.1s\n", st, st, st);
		/**/
		//pointers
		print("%p %p %p, %.2p\n", st, NULL, printf, NULL);
		print("[%20p] [%-20p]\n", NULL, NULL);
		print("[%020p] [%+-20p] [% -20p]\n", st, st, st);
		/*
		*/
		/*
		//negative cases
		print("no type % 0ratata %d\n", 13, 14);
		print("type mi%smatch\n", 100500);//segfault
		print("what if %d\n", "i put a string there");//print some integer
		*/
	}
	return (0);
}
