#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "platform.h"
#include "xtime_l.h"
#include "xil_printf.h"
#include "xparameters.h"

#define ARRAY_SIZE 2048
#define SIZE 12
#define THRESHOLD 50

int16_t sum(int16_t * array)
{
    int16_t sum = 0;
    for(int i = 0; i < ARRAY_SIZE; i++) {
        sum += array[i];
    }
    return sum;
}

void mat_product_c(float A[4][4], float B[4][4], float C[4][4]) {
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            C[i][j] = 0.0f;
            for (int k = 0; k < 4; k++) {
                C[i][j] += A[i][k] * B[k][j];
            }
        }
    }
}

void edge_c(unsigned char X[SIZE][SIZE], unsigned char Y[SIZE][SIZE]) {
    for (int i = 1; i < SIZE - 1; i++) {
        for (int j = 1; j < SIZE - 1; j++) {
            uint G = abs(X[i][j+1] - X[i][j-1]) + abs(X[i-1][j] - X[i+1][j]);
            if (G > THRESHOLD) {
                Y[i][j] = 1;
            } else {
                Y[i][j] = 0;
            }
        }
    }
}

int main()
{
    init_platform();
    printf("\n\n\n\n----------main.c----------\n\n\n\n");
    
    XTime start_cycles, end_cycles;
    XTime_SetTime(0);

    int16_t array[ARRAY_SIZE];
    for (int i = 0; i < ARRAY_SIZE; i++) {
        array[i] = 0;
    }
    array[0] = 3;
    array[1] = 2;
    array[2] = 5;

    XTime_GetTime(&start_cycles);
    int16_t sumArray = sum(array);
    XTime_GetTime(&end_cycles);

    printf("Sum of an array:\n\n");
    printf("The sum is %d\n\n", sumArray);
    printf("Sum of an array Execution time is %f ns\n", 2000000000.0 * (end_cycles - start_cycles) * (float)1/XPAR_CPU_CORTEXA9_CORE_CLOCK_FREQ_HZ);
    printf("\n\n\n");

    float A[4][4] = {
        {1.0, 2.0, 3.0, 4.0},
        {5.0, 6.0, 7.0, 8.0},
        {9.0, 10.0, 11.0, 12.0},
        {13.0, 14.0, 15.0, 16.0}
    };

    float B[4][4] = {
        {17.0, 18.0, 19.0, 20.0},
        {21.0, 22.0, 23.0, 24.0},
        {25.0, 26.0, 27.0, 28.0},
        {29.0, 30.0, 31.0, 32.0}
    };

    float C[4][4] = {0};

    XTime_GetTime(&start_cycles);
    mat_product_c(A, B, C);
    XTime_GetTime(&end_cycles);

    printf("Matrix Multiplication:\n\n");
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            printf("%f ", C[i][j]);
        }
        printf("\n");
    }
    printf("\nMatrix Multiplication Execution time is %f ns\n", 2000000000.0 * (end_cycles - start_cycles) * (float)1/XPAR_CPU_CORTEXA9_CORE_CLOCK_FREQ_HZ);
    printf("\n\n\n");

    unsigned char X[SIZE][SIZE] = {0};
    for (int i = 0; i < SIZE; i++) {
        for (int j = 0; j < SIZE; j++) {
            if (i > j) {
                X[i][j] = 100;
            } else {
                X[i][j] = 0;
            }
        }
    }

    unsigned char Y[SIZE][SIZE] = {0};

    XTime_GetTime(&start_cycles);
    edge_c(X, Y);
    XTime_GetTime(&end_cycles);

    printf("Edge Detection:\n\n");
    for (int i = 0; i < SIZE; i++) {
        for (int j = 0; j < SIZE; j++) {
            printf("%d ", Y[i][j]);
        }
        printf("\n");
    }
    printf("\nEdge Detection Execution time is %f ns\n", 2000000000.0 * (end_cycles - start_cycles) * (float)1/XPAR_CPU_CORTEXA9_CORE_CLOCK_FREQ_HZ);
    printf("\n\n\n");

    cleanup_platform();

    return 0;
}