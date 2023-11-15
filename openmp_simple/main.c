#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <omp.h>
#include <unistd.h>


#define SIZE 10

int main() {
    omp_set_num_threads(4);
    printf("Number of threads: %d\n", omp_get_max_threads());
    printf("Number of processor cores: %d\n",omp_get_num_procs());


    /* test1: thread num */
    #pragma omp parallel num_threads(2)
    {
        printf("test1 || thread %d / %d\n", omp_get_thread_num(), omp_get_num_threads());
    }


    /* test2:  for loop */
    float *p_data = (float *)malloc(sizeof(float) * SIZE * SIZE);

    struct timespec start_time, end_time;
    clock_gettime(CLOCK_MONOTONIC_RAW, &start_time);

    int i, j;
#pragma omp parallel for private(i, j)
    for (i = 0; i < SIZE; i++) {
        for (j = 0; j < SIZE; j++) {
            // printf("test2 || thread %d / %d,  calc %d, %d\n", omp_get_thread_num(), omp_get_num_threads(), i, j);
            p_data[i*SIZE + j] = i + j * 3;
            usleep(1000);
        }
    }

    clock_gettime(CLOCK_MONOTONIC_RAW, &end_time);
    float elapsed_msec = 1.0e3 * (end_time.tv_sec - start_time.tv_sec) + 1.0e-6 * (end_time.tv_nsec - start_time.tv_nsec);
    printf("time : %f [msec]\n", elapsed_msec);

    free(p_data);


    return 0;
}
