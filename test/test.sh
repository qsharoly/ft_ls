#!/bin/bash

export PATH="$PATH:$(pwd)"

if [ -d "test" ]; then
	cd test
fi

if [ ! -x "../ft_ls" ]; then
	echo "Please run from ft_ls or ft_ls/test directory."
	exit
else
	cp ../ft_ls ft_ls
fi

function test() {
	let TOTAL++
	local FTLS_OUTFILE="$OUTPATH/$TOTAL.ftls"
	local LS_OUTFILE="$OUTPATH/$TOTAL.ls"
	local STDERR_FILE="$OUTPATH/$TOTAL.stderr"

	local DIFF_FILE="$OUTPATH/$TOTAL.diff"

	ft_ls $1 > $FTLS_OUTFILE 2>$STDERR_FILE
	ls $1 > $LS_OUTFILE 2>>$STDERR_FILE

	if [[ ! -s $STDERR_FILE ]] ; then
		rm $STDERR_FILE
	fi

	if diff -u $FTLS_OUTFILE $LS_OUTFILE > $DIFF_FILE ; then
		let OK++
		rm $DIFF_FILE $FTLS_OUTFILE $LS_OUTFILE
	else
		echo Failed test $TOTAL: \"ft_ls $1\" \($2\).
	fi
}

#test cases

let OK=0
let TOTAL=0

#step into testing directory tree
WORKPATH="tree"
OUTPATH="../output"

cd $WORKPATH
rm -rf $OUTPATH
mkdir -p $OUTPATH

test "-1" "default path, single column"
test "d1" "single dir"
test "a.txt d1" "a file and a dir"
test "d1 a.txt" "a dir, then a file"
test "-1 a.txt b.txt" "multiple files"
test "-1 a.txt b.txt d1" "mix"
test "d1 d1" "two dirs"
test "-l" "default path, detailed output"
test "-1 -R" "default path, recursive"
test "-lR" "default path, recursive, detailed"
test "-R d1 dirlink" "dir and a dir link, recursive"
test "-lR d1 dirlink" "dir and a dir link, recursive, detailed"

mkdir -p "eacess"
chmod u-r "eacess"
test "-1 -R" "recursive, inaccessible directory"
test "-lR" "recursive, detailed, inaccessible directory"
rm -rf "eacess"

echo
echo $OK/$TOTAL Ok 
