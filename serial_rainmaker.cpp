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

float **grid;
float *empty;
// int height=30;
// int width=30;
// int size_threshold = 2;
// int light = 0;
// int medium = 1;
// int high = 2;
// int drizzle_mode;
using namespace std; 
int stop = 0;

// initialize grid with all 0 values
float** init_grid(int h, int w) {
    grid = (float**) malloc(sizeof(float*)*h);
    float *row = NULL;
    for (int k = 0; k < h; k++) {
        row = (float*) malloc(sizeof(float)*w);
        grid[k] = row;
        for (int i =0; i <w; i++) {
            row[i] = 0;
        }
    }
    return grid;
}

void next_iteration(float **grid, int h, int w) {
    int num;
    srand (time(NULL));
    for (int i=h-1; i >0; i--) {
        for (int j=0;j<w;j++) {
            grid[i][j] = grid[i-1][j];
        }
    }
    if (stop == 1) {
        for (int j = 0; j < w; j++)
            grid[0][j] = 0;
    } else {
        for (int j = 0; j < w; j++){
            grid[0][j] = rand() % 4;
            // drizzle = rand() % 10;
            // if (drizzle_mode == light) {
            //     if (drizzle > 6) { //30% chance of a drop formation
            //         num = rand() % 3;
            //         grid[0][j] = num+1;
            //     } 
            //     else {
            //         grid[0][j] = 0;
            //     }
            // } else if (drizzle_mode == medium) {
            //     if (drizzle >= 4) { //60% chance of a drop formation
            //         num = rand() % 3;
            //         grid[0][j] = num+1;
            //     } 
            //     else {
            //         grid[0][j] = 0;
            //     }
            // } else {
            //     if (drizzle >= 1) { //90% chance of a drop formation
            //         num = rand() % 3;
            //         grid[0][j] = num+1;
            //     } 
            //     else {
            //         grid[0][j] = 0;
            //     }
            // }
        }
    }
    
}

void set_zero(float *row, int w) {
    for (int i = 0; i < w; i++) {
        row[i] = 0;
    }
}

void split_and_merge(float **grid, int h, int w, int size_threshold) {
    for (int i=h-1; i >0; i--) {
        set_zero(empty, w);
        for (int j=0; j<w; j++) {
            if (grid[i][j] >= size_threshold) {
                if (j-1 >= 0) {
                    empty[j] += grid[i][j]/2;
                }
                if (j+1 < w) {
                    empty[j+1] += grid[i][j]/2;
                }
                // empty[j] += grid[i][j]/3;
            } else {
                empty[j] += grid[i][j];
            }
        }

        for (int j = 0;j<w; j++) {
            grid[i][j] = empty[j];
        }
    }
}

void display(float** grid, int h, int w) {
    cout << "\033[2J\033[1;1H";
    for (int i = 0; i < h; i++) {
        printf("\n");
        for (int j = 0; j < w; j++) {
            if (grid[i][j] < 1) {
                printf("    ");
            } else if (grid[i][j] < 2) {
                printf(".   ");
            } else if (grid[i][j] < 3) {
                printf("..  ");
            } else {
                printf("... ");
            } 
        }
    }
}

void serial_rainmaker(int show, int iter, int height, int width, int size_threshold, unsigned int microseconds)
{
    grid = init_grid(height, width);
    struct timeval time_start, time_end;
    long int core_logic_time=0;
    empty = (float *)malloc(sizeof(float)*width);
    // drizzle_mode = atoi(argv[2]);
    for (int i = 0; i < iter; i++) {
        if (iter - i < height) {
            stop = 1;
        }
        if (show == 1) {
            usleep(100000);
            display(grid, height, width);
        }
        gettimeofday(&time_start, NULL);
        next_iteration(grid, height, width);
        split_and_merge(grid, height, width, size_threshold);
        gettimeofday(&time_end, NULL);
        core_logic_time += (time_end.tv_sec * 1000000 + time_end.tv_usec) - (time_start.tv_sec * 1000000 + time_start.tv_usec);
    }
    if (show == 1) 
        cout << "\033[2J\033[1;1H"; 
    printf("Core logic time: %ld microseconds\n", core_logic_time);
    for (int i = 0; i < height; i++)
        free(grid[i]);
    free(empty);
}

#ifdef __cplusplus
}
#endif
