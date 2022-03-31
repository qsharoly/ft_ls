for testcase in align basic bignum dtoa fp_speed trunc wild
do
	bash test/runtest.sh $testcase
done
