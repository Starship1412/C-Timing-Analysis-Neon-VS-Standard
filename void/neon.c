#include <stdint.h>
#include <stdio.h>
#include <arm_neon.h>
#include <stdlib.h>
#include "platform.h"
#include "xtime_l.h"
#include "xil_printf.h"
#include "xparameters.h"

#define ARRAY_SIZE 2048
#define SIZE 12
#define THRESHOLD 50

int16_t sum_ni(int16_t *array)
{
    int16_t sum = 0;
    int16x8_t neonArray, acc; 
    acc = vdupq_n_s16(0);
    for(int i = 0; i < ARRAY_SIZE; i += 8) {
        neonArray = vld1q_s16(&array[i]);
        acc = vaddq_s16(neonArray, acc);
    }
    sum = vgetq_lane_s16(acc, 0);
    sum += vgetq_lane_s16(acc, 1);
    sum += vgetq_lane_s16(acc, 2);
    sum += vgetq_lane_s16(acc, 3);
    sum += vgetq_lane_s16(acc, 4);
    sum += vgetq_lane_s16(acc, 5);
    sum += vgetq_lane_s16(acc, 6);
    sum += vgetq_lane_s16(acc, 7);

    return sum;
}

void mat_product_ni(float A[4][4], float B[4][4], float C[4][4]) {
    for (int i = 0; i < 4; i++) {
        float32x4_t a_row = vld1q_f32(A[i]);
        for (int j = 0; j < 4; j++) {
            float32x4_t b_col = {B[0][j], B[1][j], B[2][j], B[3][j]};
            float32x4_t result = vmulq_f32(a_row, b_col);
            float sum = vgetq_lane_f32(result, 0) + vgetq_lane_f32(result, 1) + vgetq_lane_f32(result, 2) + vgetq_lane_f32(result, 3);
            C[i][j] = sum;
        }
    }
}

void edge_ni(unsigned char X[SIZE][SIZE], unsigned char Y[SIZE][SIZE]) {
    for (int i = 1; i < SIZE - 1; i++) {
        for (int j = 1; j < SIZE / 8 * 8; j += 8) {
            uint8x8_t east = vld1_u8(&X[i][j + 1]); // Load data using NEON
            uint8x8_t west = vld1_u8(&X[i][j - 1]);
            uint8x8_t north = vld1_u8(&X[i - 1][j]);
            uint8x8_t south = vld1_u8(&X[i + 1][j]);

            // Calculate horizontal and vertical differences
            uint8x8_t delta_h = vabd_u8(east, west); // |E - W|
            uint8x8_t delta_v = vabd_u8(north, south); // |N - S|

            uint8x8_t G = vadd_u8(delta_h, delta_v); // Sum the differences

            uint8x8_t threshold_val = vdup_n_u8(THRESHOLD); // Compare with the threshold
            uint8x8_t result = vcgt_u8(G, threshold_val); // G > THRESHOLD

            uint8x8_t one_val = vdup_n_u8(1); // Convert 255 (true) to 1
            result = vand_u8(result, one_val); // If result is 255, make it 1; otherwise 0

            vst1_u8(&Y[i][j], result); // Store result
        }
        for (int j = SIZE / 8 * 8 + 1; j < SIZE - 1; j++) {
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
    printf("\n\n\n\n----------neon.c----------\n\n\n\n");
    
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
    int16_t sum = sum_ni(array);
    XTime_GetTime(&end_cycles);

    printf("Sum of an array:\n\n");
    printf("The sum is %d\n\n", sum);
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
    mat_product_ni(A, B, C);
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
    edge_ni(X, Y);
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