#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#include <sys/time.h>

#define MATRIX_SIZE 1000

int A[MATRIX_SIZE][MATRIX_SIZE];
int B[MATRIX_SIZE][MATRIX_SIZE];
int C[MATRIX_SIZE][MATRIX_SIZE];

void initializeMatrix(int matrix[MATRIX_SIZE][MATRIX_SIZE]) {
    for (int i = 0; i < MATRIX_SIZE; i++) {
        for (int j = 0; j < MATRIX_SIZE; j++) {
            matrix[i][j] = rand() % 10;
        }
    }
}

void multiplyMatricesSequential() {
    for (int i = 0; i < MATRIX_SIZE; i++) {
        for (int j = 0; j < MATRIX_SIZE; j++) {
            for (int k = 0; k < MATRIX_SIZE; k++) {
                C[i][j] += A[i][k] * B[k][j];
            }
        }
    }
}

void multiplyMatricesParallel() {
#pragma omp parallel for
    for (int i = 0; i < MATRIX_SIZE; i++) {
        for (int j = 0; j < MATRIX_SIZE; j++) {
            for (int k = 0; k < MATRIX_SIZE; k++) {
                C[i][j] += A[i][k] * B[k][j];
            }
        }
    }
}

int main() {
    
    initializeMatrix(A);
    initializeMatrix(B);

    struct timeval start_time, end_time;
    gettimeofday(&start_time, NULL);

    multiplyMatricesSequential();

    gettimeofday(&end_time, NULL);
    double seq_time = (end_time.tv_sec - start_time.tv_sec) + (end_time.tv_usec - start_time.tv_usec) / 1000000.0;

    printf("Sequential algorithm took %f seconds\n", seq_time);

   
    gettimeofday(&start_time, NULL);

    multiplyMatricesParallel();

    gettimeofday(&end_time, NULL);
    double par_time = (end_time.tv_sec - start_time.tv_sec) + (end_time.tv_usec - start_time.tv_usec) / 1000000.0;

    printf("Parallel algorithm took %f seconds\n", par_time);

    
    double speedup = seq_time / par_time;
    double efficiency = speedup / omp_get_max_threads();
    printf("Speedup factor: %f\n", speedup);
    printf("Efficiency: %f\n", efficiency);

    return 0;
}
