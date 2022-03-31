if [[ $1 ]]; then
	EXECUTABLE="test/bin/$1.out"
	SOURCE="test/cases/$1.c"
	LFT_DIR=${LFT_DIR:-libft}
	rm $EXECUTABLE
	gcc -g $SOURCE -Iincludes -I$LFT_DIR/includes -L. -lm -lftprintf -o $EXECUTABLE
	if [[ $2 == "--norun" ]]; then
		exit
	fi
	if [[ !( -f $EXECUTABLE ) ]]; then
		exit
	fi
	if [[ $1 == "fp_speed" ]]; then
		echo "=== dtoa speed ==="
		echo "libc printf:"
		time ./$EXECUTABLE -libc 1>/dev/null
		echo
		echo "ft_printf:"
		time ./$EXECUTABLE 1>/dev/null
	else
		./$EXECUTABLE
	fi
fi
