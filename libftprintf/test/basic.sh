rm test/basic.out
gcc -g test/basic.c -L. -lftprintf -Iincludes -o test/basic.out
./test/basic.out
