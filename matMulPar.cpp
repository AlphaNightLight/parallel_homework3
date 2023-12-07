#include <mpi.h>

#include <iostream>
#include <fstream>
#include <iomanip>

#include <stdlib.h>
#include <time.h>

//#include <chrono>

#define MASTER 0

#define N_TRIALS 1
// To reduce spikes an averege will be performed

using namespace std;

typedef struct Matrix
{
	float** vals;
	int rows;
	int cols;
} Matrix;

typedef struct mat_and_time
{
	Matrix M;
	float execution_time;
} mat_and_time;

Matrix allocate_matrix(int, int);
void deallocate_matrix(Matrix);

Matrix random_dense_matrix(int, int);
mat_and_time matMulPar(Matrix, Matrix);
void subMatMulPar(int, Matrix, Matrix);
void multiply(Matrix, Matrix, Matrix);

void print_matrix(Matrix, string);
void print_matrix_ofstream(Matrix, string, ofstream);

int main(int argc, char** argv)
{
	srand(time(NULL));
	
	int my_rank, size;
	int i, j;
	
	MPI_Init(&argc, &argv);
	MPI_Comm_size(MPI_COMM_WORLD, &size);
	MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
	
	float execution_time;
	int ROW_N_A, COL_N_A, COL_N_B;
	// For the matrices to be product compatible, if the first is ROW_N_A x COL_N_A,
	// the second must be COL_N_A x COL_N_B.
	
	for (i=0;i<1;++i){//(i=0;i<3;++i){
		/*switch(i){
			case 0:
				ROW_N_A = 32;
				COL_N_A = 256;
				COL_N_B = 32;
				break;
			case 1:
				ROW_N_A = 128;
				COL_N_A = 16;
				COL_N_B = 128;
				break;
			case 2:
				ROW_N_A = 64;
				COL_N_A = 64;
				COL_N_B = 64;
				break;
		}*/
		
		ROW_N_A = 4;
		COL_N_A = 4;
		COL_N_B = 4;
		execution_time = 0.0;
		
		if (COL_N_B % size != 0){
			if(my_rank == MASTER){
				cout << "Error: matrix size not compatible with thread number!" << endl;
			}
			MPI_Finalize();
			return 1;
		}
		
		for (j=0;j<N_TRIALS;++j){
			if (my_rank == MASTER){
				Matrix A = random_dense_matrix(ROW_N_A, COL_N_A);
				print_matrix(A, "A"); // Debug
				Matrix B = random_dense_matrix(COL_N_A, COL_N_B);
				print_matrix(B, "B"); // Debug
				
				mat_and_time C_struct = matMulPar(A, B);
				
				Matrix C = C_struct.M;
				print_matrix(C, "C"); // Debug
				
				execution_time += C_struct.execution_time * (1.0 / N_TRIALS);
				
				deallocate_matrix(A);
				deallocate_matrix(B);
				deallocate_matrix(C);
			} else {
				Matrix A = random_dense_matrix(ROW_N_A, COL_N_A);
				Matrix B = random_dense_matrix(COL_N_A, COL_N_B/size);
				
				subMatMulPar(my_rank, A, B);
				
				deallocate_matrix(A);
				deallocate_matrix(B);
			}
		}
		
		if (my_rank == MASTER){
			//ofstream report_file("reports/serial/report_matMulPar.csv", std::ios_base::app);
			ofstream report_file("report_matMulPar.csv", std::ios_base::app);
			report_file << fixed << setprecision(6);
			report_file << ROW_N_A << "," << COL_N_A << "," << COL_N_B << "," << execution_time << endl;
			report_file.close();
		}
	}
	
	MPI_Finalize();
	return 0;
}

Matrix allocate_matrix(int rows, int cols)
{
	Matrix M;
	M.rows = rows;
	M.cols = cols;
	int i;
	
	M.vals = new float*[rows];
	for (i=0;i<rows;++i){
		M.vals[i] = new float[cols];
	}
	
	return M;
}

void deallocate_matrix(Matrix M)
{
	int i;
	for (i=0;i<M.rows;++i){
		delete[] M.vals[i];
	}
	delete[] M.vals;
}

Matrix random_dense_matrix(int rows, int cols)
{
	Matrix M;
	M = allocate_matrix(rows, cols);
	int i, j;
	
	for (i=0;i<rows;++i){
		for (j=0;j<cols;++j){
			M.vals[i][j] = (float)rand() / (float)rand();
		}
	}
	
	return M;
}

mat_and_time matMulPar(Matrix A, Matrix B)
{
	Matrix C;
	C = allocate_matrix(A.rows, B.cols);
	double start_time, end_time;
	float execution_time = 0.0;
	
	//auto start_time = chrono::high_resolution_clock::now();
	start_time = MPI_Wtime();
	
	multiply(A, B, C);
	
	//auto end_time = chrono::high_resolution_clock::now();
	//auto difference_time = chrono::duration_cast<chrono::microseconds>(end_time - start_time);
	//execution_time = difference_time.count() * 1e-6;
	end_time = MPI_Wtime();
	// To be coherent with the serial cases, I convert execution_time to float
	execution_time = (float)(end_time-start_time);
	
	mat_and_time retval;
	retval.M = C;
	retval.execution_time = execution_time;
	return retval;
}

void subMatMulPar(int my_rank, Matrix A, Matrix B)
{
	/* Debug */
	string name = "debug_matMulPar_rank_";
	name += to_string(my_rank);
	name += ".txt";
	
	ofstream debug_file(name.c_str(), std::ios_base::app);
	debug_file << "Hello, I'm rank " << my_rank << "!" << endl;
	
	print_matrix_ofstream(A, "A", debug_file);
	print_matrix_ofstream(B, "B", debug_file);
	/**/
	
	Matrix C;
	C = allocate_matrix(A.rows, B.cols);
	
	multiply(A, B, C);
	
	/* Debug */
	print_matrix_ofstream(C, "C", debug_file);
	debug_file.close();
	/**/
	
	deallocate(C);
}

void multiply(Matrix A, Matrix B, Matrix C){
	int depth;
	int i, j, k;
	
	if (A.cols == B.rows){
		depth = A.cols;
		
		for (i=0;i<C.rows;++i){
			for (j=0;j<C.cols;++j){
				C.vals[i][j] = 0.0;
				for(k=0;k<depth;++k){
					C.vals[i][j] += A.vals[i][k] * B.vals[k][j];
				}
			}
		}
	} else {
		cout << "Error: not compatible matrices!" << endl;
	}
}

void print_matrix(Matrix M, string name)
{
	int i, j;
	cout << fixed << setprecision(5);
	cout << "Matrix " << name << " " << M.rows << " x " << M.cols << endl;
	
	for (i=0;i<M.rows;++i){
		for (j=0;j<M.cols;++j){
			cout << M.vals[i][j] << " ";
		}
		cout << endl;
	}
	cout << endl;
}

void print_matrix_ofstream(Matrix M, string name, ofstream os)
{
	int i, j;
	os << fixed << setprecision(5);
	os << "Matrix " << name << " " << M.rows << " x " << M.cols << endl;
	
	for (i=0;i<M.rows;++i){
		for (j=0;j<M.cols;++j){
			os << M.vals[i][j] << " ";
		}
		os << endl;
	}
	os << endl;
}
