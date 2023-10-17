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
	if [[ $1 == "general_speed" ]]; then
		echo "=== general speed ===" >general_speed.txt
		echo "libc printf:" >>general_speed.txt
		/usr/bin/time -p -o general_speed.txt -a ./$EXECUTABLE -libc
		echo "ft_printf:" >>general_speed.txt
		/usr/bin/time -p -o general_speed.txt -a ./$EXECUTABLE
		cat general_speed.txt
	elif [[ $1 == "fp_speed" ]]; then
		if (( $# > 1 )); then
			OPTIONS="-iter $2"
		fi
		echo "=== floating point speed ===" >fp_speed.txt
		echo "libc printf:" >>fp_speed.txt
		/usr/bin/time -p -o fp_speed.txt -a ./$EXECUTABLE -libc $OPTIONS
		echo "ft_printf:" >>fp_speed.txt
		/usr/bin/time -p -o fp_speed.txt -a ./$EXECUTABLE $OPTIONS
		cat fp_speed.txt
	else
		./$EXECUTABLE
	fi
fi
