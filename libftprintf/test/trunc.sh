rm test/trunc.out
gcc -g test/trunc.c -Iincludes -L. -lm -lftprintf -o test/trunc.out
./test/trunc.out
