#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <assert.h>
#include <sys/time.h>
#include <vector>
#include <limits>
#include <iostream>
#include <unistd.h>
#define THREADS 512

#ifdef __cplusplus
extern "C"
{
#endif

using namespace std; 

float *cu_grid;

// initialize grid with all 0 values
float* cu_init_grid(int h, int w) {
    cu_grid = (float*) malloc(sizeof(float*)*h*w);
    for (int k = 0; k < h*w; k++) {
        cu_grid[k] = 0;
    }
    return cu_grid;
}

void cu_display(float* grid, int h, int w) {
    cout << "\033[2J\033[1;1H";
    for (int i = 0; i < h; i++) {
        printf("\n");
        for (int j = 0; j < w; j++) {
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

// Grid's evolve function while considering previous points
__global__ void cu_next_iteration(float *grid1, float *grid2, float* row, int h, int w, int size)
{
    int start = blockIdx.x*w;
    if (start >= w*h) {
        return;
    }

    // int drizzle;
    if (start == 0) {
        for (int i=0; i < w; i++) {
            grid2[i] = row[i];
        }
    } else {
        for (int i=start; i < start + w; i++) {
            grid2[i] = 0;
        }
        for (int i=start; i < start + w; i++) {
            if (grid1[i-w] >= size) {
                // if (j-1 >= 0) {
                grid2[i] += grid1[i-w]/2;
                // }
                if (i+1 < start + w) {
                    grid2[i+1] += grid1[i-w]/2;
                }
                // empty[j] += grid[i][j]/3;
            } else {
                grid2[i] += grid1[i-w];
            }
        }
    }
}

void cuda_rainmaker(int show, int iter, int height, int width, int size_threshold, unsigned int microseconds)
{
    int cu_height=height;
    int cu_width=width;
    int cu_size_threshold = size_threshold;
    int grid_dim = cu_height*cu_width;
    cu_grid = cu_init_grid(cu_height, cu_width);
    float *row = (float *)malloc(sizeof(float)*cu_width);
    float *device_row;
    srand (time(NULL));
    struct timeval time_start, time_end;
    long int kernel_time=0;

    // cuda 
    float *device_grid1, *device_grid2, *tmp;
    // cudaEvent_t event;
    // cudaEventCreate(&event);
    cudaMalloc((void **) &device_grid1, sizeof(float)*grid_dim);
    cudaMalloc((void **) &device_grid2, sizeof(float)*grid_dim);
    cudaMalloc((void **) &device_row, sizeof(float)*cu_width);
    cudaMemcpy(device_grid1, cu_grid, sizeof(float)*grid_dim, cudaMemcpyHostToDevice);
    // cudaMemcpy(device_grid2, cu_grid, sizeof(float)*grid_dim, cudaMemcpyHostToDevice);//
    dim3 dimGrid(cu_height);
    dim3 dimBlock(1);

    for (int i = 0; i < iter; i++) {
        // cu_display1(cu_grid, cu_height, cu_width);
        if (show == 1) {
            usleep(100000); 
            cu_display(cu_grid, cu_height, cu_width);
        }
        // printf("\n");
        if (iter - i < cu_height) {
            for (int j=0; j < cu_width; j++) {
                row[j] = 0;
            // printf("%.1f, ", row[j]);
            }
        } else {
            for (int j=0; j < cu_width; j++) {
                row[j] = rand() % 4;
            // printf("%.1f, ", row[j]);
            }
        }
        cudaMemcpy(device_row, row, sizeof(float)*cu_width, cudaMemcpyHostToDevice);
        gettimeofday(&time_start, NULL);
        cu_next_iteration<<<dimGrid, dimBlock>>>(device_grid1, device_grid2, device_row, cu_height, cu_width, cu_size_threshold);
        gettimeofday(&time_end, NULL);
        kernel_time += (time_end.tv_sec * 1000000 + time_end.tv_usec) - (time_start.tv_sec * 1000000 + time_start.tv_usec);
        tmp = device_grid2;
        device_grid2 = device_grid1;
        device_grid1 = tmp;
        // cu_split_and_merge<<<dimGrid, dimBlock>>>(device_grid1, device_grid2, cu_height, cu_width, cu_size_threshold);
        // cudaMemcpy(cu_grid, device_grid2, sizeof(float)*grid_dim, cudaMemcpyDeviceToHost);  
        // cudaMemcpy(device_grid1, cu_grid, sizeof(float)*grid_dim, cudaMemcpyHostToDevice);
        // cu_split_and_merge<<<dimGrid, dimBlock>>>(device_grid1, device_grid2, cu_height, cu_width, cu_size_threshold);
        // cudaMemcpy(device_grid2, cu_grid, sizeof(float)*grid_dim, cudaMemcpyHostToDevice);
        cudaThreadSynchronize();
        cudaMemcpy(cu_grid, device_grid1, sizeof(float)*grid_dim, cudaMemcpyDeviceToHost);  
    }
    // cudaThreadSynchronize();
    // cudaEventSynchronize(event);  
    if (show == 1) 
        cout << "\033[2J\033[1;1H"; 
    printf("kernel time: %ld microseconds\n", kernel_time);
    // free back to heap
    cudaFree(device_grid1);
    cudaFree(device_grid2);
    free(cu_grid);
}

#ifdef __cplusplus
}
#endif
