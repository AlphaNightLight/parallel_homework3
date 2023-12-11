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
mat_and_time matMulPar(Matrix, Matrix, int, int);
void multiply(Matrix, Matrix, Matrix);

void print_matrix(Matrix, string);
void print_matrix_ofstream(Matrix, string, ofstream&);

int main(int argc, char** argv)
{
	srand(time(NULL));
	
	int my_rank, size;
	int i;
	int scaling_type;
	
	MPI_Init(&argc, &argv);
	MPI_Comm_size(MPI_COMM_WORLD, &size);
	MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
	
	float execution_time;
	int ROW_N_A, COL_N_A, COL_N_B;
	// For the matrices to be product compatible, if the first is ROW_N_A x COL_N_A,
	// the second must be COL_N_A x COL_N_B.
	
	for (scaling_type=0;scaling_type<1;++scaling_type){//(scaling_type=0;scaling_type<2;++scaling_type){
		ROW_N_A = 4;
		COL_N_A = 4;
		COL_N_B = 4;
		execution_time = 0.0;
		
		if (scaling_type == 1){
			ROW_N_A *= size;
		}
		
		if (ROW_N_A % size != 0){
			if(my_rank == MASTER){
				cout << "Error: matrix size not compatible with thread number!" << endl;
			}
			MPI_Finalize();
			return 1;
		}
		
		for (i=0;i<N_TRIALS;++i){
			if (my_rank == MASTER){
				Matrix A = random_dense_matrix(ROW_N_A, COL_N_A);
				print_matrix(A, "A"); // Debug
				Matrix B = random_dense_matrix(COL_N_A, COL_N_B);
				print_matrix(B, "B"); // Debug
				
				mat_and_time C_struct = matMulPar(A, B, size, my_rank);
				
				Matrix C = C_struct.M;
				print_matrix(C, "C"); // Debug
				
				execution_time += C_struct.execution_time * (1.0 / N_TRIALS);
				
				deallocate_matrix(A);
				deallocate_matrix(B);
				deallocate_matrix(C);
			} else {
				Matrix A = allocate_matrix(ROW_N_A/size, COL_N_A);
				Matrix B = allocate_matrix(COL_N_A, COL_N_B);
				
				deallocate_matrix(matMulPar(A, B, size, my_rank).M);
				
				deallocate_matrix(A);
				deallocate_matrix(B);
			}
		}
		
		if (my_rank == MASTER){
			if (scaling_type == 0){
				ofstream report_file("reports/report_matMulRowsPar_strong.csv", std::ios_base::app);
			} else {
				ofstream report_file("reports/report_matMulRowsPar_weak.csv", std::ios_base::app);
			}
			report_file << fixed << setprecision(6);
			report_file << size << "," << ROW_N_A << "," << COL_N_A << "," << COL_N_B << "," << execution_time << endl;
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

mat_and_time matMulPar(Matrix A, Matrix B, int size, int my_rank)
{
	Matrix C;
	C = allocate_matrix(A.rows, B.cols);
	double start_time, end_time;
	float execution_time = 0.0;
	int i;
	
	//auto start_time = chrono::high_resolution_clock::now();
	start_time = MPI_Wtime();
	
	for (i=0;i<B.rows;++i){
		MPI_Bcast(B.vals[i], B.cols, MPI_FLOAT, MASTER, MPI_COMM_WORLD);
	}
	
	if (my_rank == MASTER){
		int dest;
		for (i=0;i<A.rows;++i){
			dest = (i * size) / A.rows; // i / (A.rows / size);
			if (dest != MASTER){
				MPI_Send(A.vals[i], A.cols, MPI_FLOAT, dest, 0, MPI_COMM_WORLD);
			}
		}
		
		if (MASTER != 0){
			int master_base = MASTER * (A.rows/size);
			for (i=0;i<A.rows/size;++i){
				A.vals[i] = A.vals[i + master_base];
			}
		}
		
		// For the master the local submatrix A is the Final A, same for C. So, we need to restrict them to the first rows.
		A.rows = A.rows / size;
		C.rows = C.rows / size;
	} else {
		for (i=0;i<A.rows;++i){
			MPI_Recv(A.vals[i], A.cols, MPI_FLOAT, MASTER, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
		}
	}
	
	/* Debug */
	string name = "debugs/debug_matMulRowsPar_rank_";
	name += to_string(my_rank);
	name += ".txt";
	
	ofstream debug_file(name.c_str(), std::ios_base::app);
	debug_file << "Hello, I'm rank " << my_rank << "!" << endl;
	
	print_matrix_ofstream(A, "A", debug_file);
	print_matrix_ofstream(B, "B", debug_file);
	/**/
	
	multiply(A, B, C);
	
	/* Debug */
	print_matrix_ofstream(C, "C", debug_file);
	debug_file.close();
	/**/
	
	if (my_rank == MASTER){
		// We remember to set A's and C's dimensions to the original ones
		A.rows = A.rows * size;
		C.rows = C.rows * size;
		
		if (MASTER != 0){
			int master_base = MASTER * (C.rows/size);
			for (i=0;i<C.rows/size;++i){
				C.vals[i + master_base] = C.vals[i];
			}
		}
		
		int source;
		for (i=0;i<C.rows;++i){
			source = (i * size) / C.rows; // i / (A.rows / size);
			if (source != MASTER){
				MPI_Recv(C.vals[i], C.cols, MPI_FLOAT, source, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
			}
		}
	} else {
		for (i=0;i<C.rows;++i){
			MPI_Send(C.vals[i], C.cols, MPI_FLOAT, MASTER, 0, MPI_COMM_WORLD);
		}
	}
	
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

void print_matrix_ofstream(Matrix M, string name, ofstream& os)
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
