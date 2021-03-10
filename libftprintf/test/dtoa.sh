rm test/dtoa.out
gcc -g test/dtoa.c -Iincludes -L. -lm -lftprintf -o test/dtoa.out
if [[ $1 == "run" ]]; then
	./test/dtoa.out;
fi
