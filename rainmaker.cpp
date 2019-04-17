#include <stdlib.h>
#include <stdio.h>
#include <float.h>
#include <sys/time.h>
#include "rainmaker.h"
#include <time.h>
#include <limits>
#include <iostream>
#include <unistd.h>

int main( int argc, char** argv)
{ 
    if (argc != 6) {
        printf("Usage: ./rainmaker <iteration count> <show rain (0/1)> <height of grid> <width of grid> <mode (0 - serial, 1 - pthread, 2 - cuda)>");
        return 1;
    }

    struct timeval time_start, time_end;

    unsigned int wait = 1000000;
    int iter = atoi(argv[1]);
    int show = atoi(argv[2]);
    int height = atoi(argv[3]);
    int width = atoi(argv[4]);
    int mode = atoi(argv[5]);
    int size_threshold = 2;
	switch(mode)
    {   
        case 1:
            printf("Rain simulation - Pthreads\n");
            usleep(wait);
            gettimeofday(&time_start, NULL);
            pthread_rainmaker(show, iter, height, width, size_threshold, wait);
            gettimeofday(&time_end, NULL);
            fprintf(stderr,"Simulation using pthreads over 4 cores: %ld microseconds\n",((time_end.tv_sec * 1000000 + time_end.tv_usec) - (time_start.tv_sec * 1000000 + time_start.tv_usec)));
            break;
        case 2: 
        	printf("Rain simulation - Cuda\n");
            usleep(wait);
            gettimeofday(&time_start, NULL);
            cuda_rainmaker(show, iter, height, width, size_threshold, wait);
            gettimeofday(&time_end, NULL);
            fprintf(stderr,"Simulation using Cuda: %ld microseconds\n",((time_end.tv_sec * 1000000 + time_end.tv_usec) - (time_start.tv_sec * 1000000 + time_start.tv_usec)));
        	break;
        case 0: 
        	printf("Rain simulation - Serial\n");
            usleep(wait);
            gettimeofday(&time_start, NULL);
            serial_rainmaker(show, iter, height, width, size_threshold, wait);
            gettimeofday(&time_end, NULL);
            fprintf(stderr,"Simulation using single core: %ld microseconds\n",((time_end.tv_sec * 1000000 + time_end.tv_usec) - (time_start.tv_sec * 1000000 + time_start.tv_usec)));
        	break;
        default:
            printf("Illegal mode for rainmaker");
            return 1;
            break;
    }
    return 0;
}
