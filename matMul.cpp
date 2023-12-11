#include <iostream>
#include <fstream>
#include <iomanip>

#include <stdlib.h>
#include <time.h>

#include <chrono>

#define N_TRIALS 3
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
mat_and_time matMul(Matrix, Matrix);

void print_matrix(Matrix, string);

int main(int argc, char** argv)
{
	srand(time(NULL));
	ofstream report_file("reports/report_matMul.csv", std::ios_base::app);
	float execution_time;
	int i;
	
	int ROW_N_A, COL_N_A, COL_N_B;
	// For the matrices to be product compatible, if the first is ROW_N_A x COL_N_A,
	// the second must be COL_N_A x COL_N_B.
	
	ROW_N_A = 512;
	COL_N_A = 512;
	COL_N_B = 512;
	execution_time = 0.0;
	
	for (i=0;i<N_TRIALS;++i){
		Matrix A = random_dense_matrix(ROW_N_A, COL_N_A);
		//print_matrix(A, "A"); // Debug
		Matrix B = random_dense_matrix(COL_N_A, COL_N_B);
		//print_matrix(B, "B"); // Debug
		
		mat_and_time C_struct = matMul(A, B);
		Matrix C = C_struct.M;
		//print_matrix(C, "C"); // Debug
		
		execution_time += C_struct.execution_time * (1.0 / N_TRIALS);
		
		deallocate_matrix(A);
		deallocate_matrix(B);
		deallocate_matrix(C);
	}
	
	report_file << fixed << setprecision(6);
	report_file << ROW_N_A << "," << COL_N_A << "," << COL_N_B << "," << execution_time << endl;
	
	report_file.close();
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

mat_and_time matMul(Matrix A, Matrix B)
{
	Matrix C;
	C = allocate_matrix(A.rows, B.cols);
	float execution_time = 0.0;
	int depth;
	int i, j, k;
	
	if (A.cols == B.rows){
		depth = A.cols;
		auto start_time = chrono::high_resolution_clock::now();
		
		for (i=0;i<C.rows;++i){
			for (j=0;j<C.cols;++j){
				C.vals[i][j] = 0.0;
				for(k=0;k<depth;++k){
					C.vals[i][j] += A.vals[i][k] * B.vals[k][j];
				}
			}
		}
		
		auto end_time = chrono::high_resolution_clock::now();
		auto difference_time = chrono::duration_cast<chrono::microseconds>(end_time - start_time);
		execution_time = difference_time.count() * 1e-6;
	} else {
		cout << "Error: not compatible matrices!" << endl;
	}
	
	mat_and_time retval;
	retval.M = C;
	retval.execution_time = execution_time;
	return retval;
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
