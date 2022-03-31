for testcase in align basic bignum dtoa fp_speed trunc wild
do
	bash test/test.sh $testcase
done
