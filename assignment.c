#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

#define IMG_WIDTH 800
#define IMG_HEIGHT 800
#define MAX_ITERATIONS 1000

int mandelbrot(double x, double y) {
    double real = 0.0, imag = 0.0;
    int iterations = 0;
    while (real * real + imag * imag <= 4.0 && iterations < MAX_ITERATIONS) {
        double temp = real * real - imag * imag + x;
        imag = 2.0 * real * imag + y;
        real = temp;
        iterations++;
    }
    return iterations;
}

int main(int argc, char** argv) {
    MPI_Init(&argc, &argv);

    int proc_rank, proc_size;
    MPI_Comm_rank(MPI_COMM_WORLD, &proc_rank);
    MPI_Comm_size(MPI_COMM_WORLD, &proc_size);

    int rows_per_proc = IMG_HEIGHT / proc_size;
    int start_row = proc_rank * rows_per_proc;
    int end_row = (proc_rank == proc_size - 1) ? IMG_HEIGHT : (proc_rank + 1) * rows_per_proc;

    int *image = (int*)malloc(IMG_WIDTH * (end_row - start_row) * sizeof(int));

    double start_time = MPI_Wtime();
    for (int j = start_row; j < end_row; j++) {
        for (int i = 0; i < IMG_WIDTH; i++) {
            double x = (i - IMG_WIDTH/2.0) * 4.0 / IMG_WIDTH;
            double y = (j - IMG_HEIGHT/2.0) * 4.0 / IMG_WIDTH;
            image[(j - start_row) * IMG_WIDTH + i] = mandelbrot(x, y);
        }
    }
    double end_time = MPI_Wtime();
    double parallel_time = end_time - start_time;

    int *globalImage = NULL;
    if (proc_rank == 0) {
        globalImage = (int*)malloc(IMG_WIDTH * IMG_HEIGHT * sizeof(int));
    }

    MPI_Gather(image, IMG_WIDTH * (end_row - start_row), MPI_INT, globalImage, IMG_WIDTH * (end_row - start_row), MPI_INT, 0, MPI_COMM_WORLD);

    if (proc_rank == 0) {
        // Calculate sequential time for comparison
        double seq_start_time = MPI_Wtime();
        for (int j = 0; j < IMG_HEIGHT; j++) {
            for (int i = 0; i < IMG_WIDTH; i++) {
                double x = (i - IMG_WIDTH/2.0) * 4.0 / IMG_WIDTH;
                double y = (j - IMG_HEIGHT/2.0) * 4.0 / IMG_WIDTH;
                globalImage[j * IMG_WIDTH + i] = mandelbrot(x, y);
            }
        }
        double seq_end_time = MPI_Wtime();
        double sequential_time = seq_end_time - seq_start_time;

        printf("Sequential time: %f seconds\n", sequential_time);
        printf("Parallel time: %f seconds\n", parallel_time);
        free(globalImage);
    }

    free(image);
    MPI_Finalize();

    return 0;
}
