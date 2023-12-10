if [ ! -d bin ];
then
	mkdir bin
fi



g++ -std=gnu++11 -o bin/matMul.exe matMul.cpp

mpicxx -o bin/matMulRowsPar.exe matMulRowsPar.cpp
mpicxx -o bin/matMulColsPar.exe matMulColsPar.cpp
mpicxx -o bin/matMulBlockPar.exe matMulBlockPar.cpp
