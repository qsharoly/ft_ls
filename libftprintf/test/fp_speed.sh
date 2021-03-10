rm -f test/fp_speed.out
gcc -pg -Wall -Wextra -Werror test/fp_speed.c -Iincludes -L. -lm -lftprintf -o test/fp_speed.out

if [[ $1 == "run" && -f "./test/fp_speed.out" ]]; then
	echo "libc printf:"
	time ./test/fp_speed.out -libc 1>/dev/null
	echo
	echo "ft_printf:"
	time ./test/fp_speed.out 1>/dev/null
fi
