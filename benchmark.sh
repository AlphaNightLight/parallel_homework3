if [ ! -d reports ];
then
	mkdir reports
fi

if [ ! -d debugs ];
then
	mkdir debugs
fi



echo >"reports/report_matMul.csv" ROW_N_A,COL_N_A,COL_N_B,time
./bin/matMul.exe

echo >"reports/report_matMulRowsPar_strong.csv" SIZE,ROW_N_A,COL_N_A,COL_N_B,time
echo >"reports/report_matMulRowsPar_weak.csv" SIZE,ROW_N_A,COL_N_A,COL_N_B,time
#mpiexec -np 1 ./bin/matMulRowsPar.exe
#mpiexec -np 2 ./bin/matMulRowsPar.exe
mpiexec -np 4 ./bin/matMulRowsPar.exe
#mpiexec -np 8 ./bin/matMulRowsPar.exe
#mpiexec -np 16 ./bin/matMulRowsPar.exe
#mpiexec -np 32 ./bin/matMulRowsPar.exe
#mpiexec -np 64 ./bin/matMulRowsPar.exe
#mpiexec -np 128 ./bin/matMulRowsPar.exe

echo >"reports/report_matMulColsPar_strong.csv" SIZE,ROW_N_A,COL_N_A,COL_N_B,time
echo >"reports/report_matMulColsPar_weak.csv" SIZE,ROW_N_A,COL_N_A,COL_N_B,time
#mpiexec -np 1 ./bin/matMulColsPar.exe
#mpiexec -np 2 ./bin/matMulColsPar.exe
mpiexec -np 4 ./bin/matMulColsPar.exe
#mpiexec -np 8 ./bin/matMulColsPar.exe
#mpiexec -np 16 ./bin/matMulColsPar.exe
#mpiexec -np 32 ./bin/matMulColsPar.exe
#mpiexec -np 64 ./bin/matMulColsPar.exe
#mpiexec -np 128 ./bin/matMulColsPar.exe

echo >"reports/report_matMulBlockPar_strong.csv" SIZE,ROW_N_A,COL_N_A,COL_N_B,time
echo >"reports/report_matMulBlockPar_weak.csv" SIZE,ROW_N_A,COL_N_A,COL_N_B,time
#mpiexec -np 1 ./bin/matMulBlockPar.exe
#mpiexec -np 2 ./bin/matMulBlockPar.exe
mpiexec -np 4 ./bin/matMulBlockPar.exe
#mpiexec -np 8 ./bin/matMulBlockPar.exe
#mpiexec -np 16 ./bin/matMulBlockPar.exe
#mpiexec -np 32 ./bin/matMulBlockPar.exe
#mpiexec -np 64 ./bin/matMulBlockPar.exe
#mpiexec -np 128 ./bin/matMulBlockPar.exe
