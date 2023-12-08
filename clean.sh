rm -f matMulBlockPar.out
rm -f report_matMulPar.csv

for i in {0..3};
do
	rm -f debug_matMulPar_rank_$i.txt
done
