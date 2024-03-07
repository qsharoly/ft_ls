if [[ $1 ]]; then
	if [[ !( $CC ) ]]; then
		CC="clang"
	fi
	if [[ !( $CCFLAGS ) ]]; then
		CCFLAGS="-O2 -g"
	fi
	mkdir -p test/bin #ensure bin directory exists
	BIN="test/bin/$1.out"
	SRC="test/cases/$1.c"
	LFT_DIR=${LFT_DIR:-libft}
	$CC $CCFLAGS $SRC -Iincludes -I$LFT_DIR/includes -L. -lm -lftprintf -o $BIN
	if [[ $? != 0 ]]; then
		exit
	fi
	if [[ $2 == "--norun" ]]; then
		exit
	fi
	if [[ !( -f $BIN ) ]]; then
		exit
	fi
	TIMESTAMP=$(date +"%d/%m/%y %T")
	if [[ $1 == "general_speed" ]]; then
		echo -e "\n$TIMESTAMP === general speed ($CC $CCFLAGS) ===" >>general_speed.txt
		echo "libc printf:" >>general_speed.txt
		/usr/bin/time -p -o general_speed.txt -a ./$BIN -libc
		echo "ft_printf:" >>general_speed.txt
		/usr/bin/time -p -o general_speed.txt -a ./$BIN
		cat general_speed.txt
	elif [[ $1 == "fp_speed" ]]; then
		if (( $# > 1 )); then
			OPTIONS="-iter $2"
		fi
		echo -e "\n$TIMESTAMP === floating point speed ===" >>fp_speed.txt
		echo "libc printf:" >>fp_speed.txt
		/usr/bin/time -p -o fp_speed.txt -a ./$BIN -libc $OPTIONS
		echo "ft_printf:" >>fp_speed.txt
		/usr/bin/time -p -o fp_speed.txt -a ./$BIN $OPTIONS
		cat fp_speed.txt
	else
		./$BIN
	fi
fi
