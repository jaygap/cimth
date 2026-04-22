#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../inc/convolution.h"

unsigned char* applyKernelToImageSingleThread(unsigned char* original, int* kernel, int kernel_size, int width, int height, int passes){

    unsigned char* altered = (unsigned char*)malloc(width * height * 4);

    if (!altered){
        printf("Failed to allocate for image in heap.");
        return NULL;
    }

    for (int p = 0; p < passes; p++){
        for (int y = 0; y < height; y++){
            for (int x = 0; x < width; x++){
                if (p % 2 == 0){
                    applyKernelToPixel(original, altered, kernel, kernel_size, x, y, width, height);
                } else{
                    applyKernelToPixel(altered, original, kernel, kernel_size, x, y, width, height);
                }
            }
        }
    }

    if (passes % 2 == 1){
        memcpy(original, altered, width * height * 4);
    }

    free(altered);

    return original;
}

//TODO MAKE THIS MORE GENERIC
void applyKernelToPixel(unsigned char* original, unsigned char* altered, int* kernel, int kernel_size, int col, int row, int width, int height){
    unsigned int red = 0, green = 0, blue = 0, alpha = 0, pixel_count = 0;

    for (int y = -kernel_size; y <= kernel_size; y++){
        for (int x = -kernel_size; x <= kernel_size; x++){
            if (col + x >= 0 && col + x < width && row + y >= 0 && row + y < height){
                red += original[(col + x + (row + y) * width) * 4] * kernel[x + kernel_size + (y + kernel_size) * kernel_size];
                green += original[(col + x + (row + y) * width) * 4 + 1] * kernel[x + kernel_size + (y + kernel_size) * kernel_size];
                blue += original[(col + x + (row + y) * width) * 4 + 2] * kernel[x + kernel_size + (y + kernel_size) * kernel_size];
                alpha += original[(col + x + (row + y) * width) * 4 + 3] * kernel[x + kernel_size + (y + kernel_size) * kernel_size];
                pixel_count += 1 * kernel[x + kernel_size + (y + kernel_size) * kernel_size];
            }
        }
    }

    red /= pixel_count;
    green /= pixel_count;
    blue /= pixel_count;
    alpha /= pixel_count;

    altered[(col + row * width) * 4] = red;
    altered[(col + row * width) * 4 + 1] = green;
    altered[(col + row * width) * 4 + 2] = blue;
    altered[(col + row * width) * 4 + 3] = alpha;
}

unsigned char* boxBlur(unsigned char* image, struct OperationState state){
    //arg1 of OperationState is kernel_size and arg2 is number of passes to perform

    int total_kernel_size = 4 * state.arg1 * state.arg1 + 4 * state.arg1 + 1;
    int kernel[total_kernel_size];

    //set kernel contents to be all 1 as every pixel in box blur has same weighting
    for (int i = 0; i < total_kernel_size; i++){
        kernel[i] = 1;
    }

    switch (state.algo){
        case MULTI_THREAD: break;
        case GPU_ACCELERATED: break;
        default: return applyKernelToImageSingleThread(image, kernel, state.arg1, state.width, state.height, state.arg2);
    }

    return NULL;
}

unsigned char* gaussianBlur(unsigned char* image, struct OperationState state){
    //arg1 of OperationState is kernel_size and arg2 is number of passes to perform

    int total_kernel_size = 4 * state.arg1 * state.arg1 + 4 * state.arg1 + 1;
    int kernel[total_kernel_size];

    //gaussian kernel generation, probably more efficient ways to do this
    for (int y = 0; y < state.arg1 * 2 + 1; y++){
        if (y == 0){
            kernel[0] = 1;
        } else if (y <= state.arg1){
            kernel[y * (state.arg1 * 2 + 1)] = kernel[y * (state.arg1 * 2 + 1) - 1] * 2;
        } else{
            kernel[y * (state.arg1 * 2 + 1)] = kernel[y * (state.arg1 * 2 + 1) -1] / 2;
        }

        for (int x = 1; x < state.arg1 * 2 + 1; x++){
            if (x <= state.arg1){
                kernel[x + y * (state.arg1 * 2 + 1)] = 2 * kernel[x + y * (state.arg1 * 2 + 1) - 1];
            } else{
                kernel[x + y * (state.arg1 * 2 + 1)] = kernel[x + y * (state.arg1 * 2 + 1) - 1] / 2;
            }
        }
    }

    switch (state.algo){
        case MULTI_THREAD: break;
        case GPU_ACCELERATED: break;
        default: return applyKernelToImageSingleThread(image, kernel, state.arg1, state.width, state.height, state.arg2);
    }

    return NULL;
}
