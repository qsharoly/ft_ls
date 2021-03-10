rm test/wild.out
gcc -g test/wild.c -L. -lftprintf -Iincludes -o test/wild.out
./test/wild.out
