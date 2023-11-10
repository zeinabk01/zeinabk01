#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/time.h>

#define MATRIX_SIZE 1000
#define num_threads 4

int matrixA[MATRIX_SIZE][MATRIX_SIZE];
int matrixB[MATRIX_SIZE][MATRIX_SIZE];
int resultMatrix[MATRIX_SIZE][MATRIX_SIZE];

void* multiplyMatrix(void* arg) {
    int threadID = *(int*)arg;

    int startRow = threadID * MATRIX_SIZE / num_threads;
    int endRow = (threadID + 1) * MATRIX_SIZE / num_threads;

    for (int i = startRow; i < endRow; i++) {
        for (int j = 0; j < MATRIX_SIZE; j++) {
            for (int k = 0; k < MATRIX_SIZE; k++) {
                resultMatrix[i][j] += matrixA[i][k] * matrixB[k][j];
            }
        }
    }

    pthread_exit(NULL);
}

int main() {
    
    for (int i = 0; i < MATRIX_SIZE; i++) {
        for (int j = 0; j < MATRIX_SIZE; j++) {
            matrixA[i][j] = rand() % 10;
            matrixB[i][j] = rand() % 10;
            resultMatrix[i][j] = 0;
        }
    }

    struct timeval startTime, endTime;
    gettimeofday(&startTime, NULL);

    for (int i = 0; i < MATRIX_SIZE; i++) {
        for (int j = 0; j < MATRIX_SIZE; j++) {
            for (int k = 0; k < MATRIX_SIZE; k++) {
                resultMatrix[i][j] += matrixA[i][k] * matrixB[k][j];
            }
        }
    }

    gettimeofday(&endTime, NULL);
    double sequentialTime = (endTime.tv_sec - startTime.tv_sec) + (endTime.tv_usec - startTime.tv_usec) / 1000000.0;

    printf("Sequential algorithm took %f seconds\n", sequentialTime);

    gettimeofday(&startTime, NULL);

    pthread_t threads[num_threads];
    int threadIDs[num_threads];

    for (int i = 0; i < num_threads; i++) {
        threadIDs[i] = i;
        pthread_create(&threads[i], NULL, multiplyMatrix, (void*)&threadIDs[i]);
    }

    for (int i = 0; i < num_threads; i++) {
        pthread_join(threads[i], NULL);
    }

    gettimeofday(&endTime, NULL);
    double parallelTime = (endTime.tv_sec - startTime.tv_sec) + (endTime.tv_usec - startTime.tv_usec) / 1000000.0;

    printf("Parallel algorithm took %f seconds\n", parallelTime);

    double speedup = sequentialTime / parallelTime;
    double efficiency = speedup / num_threads;
    printf("Speedup factor: %f\n", speedup);
    printf("Efficiency: %f\n", efficiency);

    return 0;
}
