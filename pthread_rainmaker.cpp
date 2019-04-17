#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <assert.h>
#include <sys/time.h>
#include <vector>
#include <limits>
#include <iostream>
#include <unistd.h>
#include <pthread.h>

#define THREADS 512

#ifdef __cplusplus
extern "C"
{
#endif

float *grid1, *grid2;
float *row;
int pthread_height;
int pthread_width;
const int NTHREADS = 4;
int pthread_size_threshold;
using namespace std;

// initialize grid with all 0 values
float* pthread_init_grid(int h, int w) {
    int dim = h*w;
    float *g = (float*) malloc(sizeof(float)*dim);
    for (int k = 0; k < dim; k++) {
        g[k] = 0;
    }
    return g;
}

void* pthread_next_iteration(void *arg) {
    int *id = (int *)arg;
    int h = pthread_height;
    int w = pthread_width;
    int num;
    int segment = pthread_height/NTHREADS;
    int start = (*id)*segment;
    int end = (*id+1)*segment;
    if (end > h) {
        end = h;
    }
    for (int i = start; i < end; i++) {
        if (i==0) {
            for (int j = 0; j < w; j++) {
                grid2[j] = row[j];
            }
        } else {
            for (int j = 0; j < w; j++)
                grid2[i*w+j]= 0;

            for (int j = 0; j < w; j++) {
                if (pthread_size_threshold <= grid1[(i-1)*w+j]) {
                    grid2[i*w+j] += grid1[(i-1)*w+j]/2;
                    if (j+1 < w) {
                        grid2[i*w+j+1] += grid1[(i-1)*w+j]/2;
                    }
                } else {
                    grid2[i*w+j] += grid1[(i-1)*w+j];
                }
            }
        }
    }
}


void pthread_display(float* grid, int h, int w) {
    cout << "\033[2J\033[1;1H";
    for (int i = 0; i < h; i++) {
        printf("\n");
        for (int j = 0; j < w; j++) {
            // printf("%d, %d, %f\n", i, j, grid[i*w+j]);
            if (grid[i*w+j] < 1) {
                printf("    ");
            } else if (grid[i*w+j] < 2) {
                printf(".   ");
            } else if (grid[i*w+j] < 3) {
                printf("..  ");
            } else {
                printf("... ");
            }
        }
    }
}

void pthread_rainmaker(int show, int iter, int height, int width, int size_threshold, unsigned int microseconds)
{
    srand (time(NULL));
    float *tmp;    
    struct timeval time_start, time_end;
    long int core_logic_time=0;
    pthread_size_threshold = size_threshold;
    pthread_height = height;
    pthread_width = width;
    grid1 = pthread_init_grid(height, width);
    grid2 = pthread_init_grid(height, width);
    row = (float *)malloc(sizeof(float)*width);
    pthread_t pthread[4];
    int *ids = (int*)malloc(sizeof(int)*NTHREADS);
    for (int i = 0; i < NTHREADS; i++){
        ids[i] = i;
    }

    for (int i = 0; i < iter; i++) {
        if (show == 1) {
            usleep(100000); 
            pthread_display(grid1, height, width);
        }
        if (iter - i > height) {
            for (int j=0; j < width; j++) {
                row[j] = rand() % 4;
                // printf("%.1f, ", row[j]);
            }
        } else {
            for (int j=0; j < width; j++) {
                row[j] = 0;
                // printf("%.1f, ", row[j]);
            }
        }
        
        gettimeofday(&time_start, NULL);
        for (int j = 0; j < NTHREADS; j++) {
            pthread_create(&pthread[j], NULL, pthread_next_iteration, (void*)(ids+j)); 
        }
        for (int j =0; j < NTHREADS; j++){
            pthread_join(pthread[j], NULL);
        }
        gettimeofday(&time_end, NULL);
        core_logic_time += (time_end.tv_sec * 1000000 + time_end.tv_usec) - (time_start.tv_sec * 1000000 + time_start.tv_usec);
        tmp = grid2;
        grid2 = grid1;
        grid1 = tmp;
    }
    if (show == 1) 
        cout << "\033[2J\033[1;1H"; 
    printf("Core logic time: %ld microseconds\n", core_logic_time);
    free(grid1);
    free(grid2);
    free(row);
}

#ifdef __cplusplus
}
#endif
