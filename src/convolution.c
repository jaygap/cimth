//convolutions include blurs and edge-detection algorithms

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "../inc/convolution.h"
#include "../inc/pixel_operations.h"
#include "../inc/mask.h"

unsigned char* applyKernelSingleThread(unsigned char* original, void (*func)(unsigned char*, unsigned char*, int*, int, int, int, int, int), int* kernel, int kernel_size, int width, int height, int passes){

    unsigned char* altered = (unsigned char*)malloc(width * height * 4);

    if (!altered){
        printf("Failed to allocate memory for image in heap.");
        return NULL;
    }

    for (int p = 0; p < passes; p++){
        for (int y = 0; y < height; y++){
            for (int x = 0; x < width; x++){
                if (p % 2 == 0){
                    func(original, altered, kernel, kernel_size, x, y, width, height);
                } else{
                    func(altered, original, kernel, kernel_size, x, y, width, height);
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

void blurPixel(unsigned char* original, unsigned char* altered, int* kernel, int kernel_size, int col, int row, int width, int height){
    unsigned int red = 0, green = 0, blue = 0, alpha = 0, pixel_count = 0;
    const int kernel_width = 2 * kernel_size + 1;

    for (int y = -kernel_size; y <= kernel_size; y++){
        for (int x = -kernel_size; x <= kernel_size; x++){
            if (col + x >= 0 && col + x < width && row + y >= 0 && row + y < height){
                red += original[(col + x + (row + y) * width) * 4] * kernel[x + kernel_size + (y + kernel_size) * kernel_width];
                green += original[(col + x + (row + y) * width) * 4 + 1] * kernel[x + kernel_size + (y + kernel_size) * kernel_width];
                blue += original[(col + x + (row + y) * width) * 4 + 2] * kernel[x + kernel_size + (y + kernel_size) * kernel_width];
                alpha += original[(col + x + (row + y) * width) * 4 + 3] * kernel[x + kernel_size + (y + kernel_size) * kernel_width];
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
        default: return applyKernelSingleThread(image, &blurPixel, kernel, state.arg1, state.width, state.height, state.arg2);
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
        default: return applyKernelSingleThread(image, &blurPixel, kernel, state.arg1, state.width, state.height, state.arg2);
    }

    return NULL;
}

//TODO FIX ME PLEASE!!!
// not detecting edges

void detectPixelEdge(unsigned char* original, unsigned char* altered, int* kernel, int kernel_size, int col, int row, int width, int height){
    int red = 0, green = 0, blue = 0, alpha = 0, pixel_count = 0;
    int weight = 0;
    const int kernel_width = 2 * kernel_size + 1;

    for (int y = -kernel_size; y <= kernel_size; y++){
        for (int x = -kernel_size; x <= kernel_size; x++){
            if (col + x >= 0 && col + x < width && row + y >= 0 && row + y < height){
                red = original[(col + x + (row + y) * width) * 4];
                green = original[(col + x + (row + y) * width) * 4 + 1];
                blue = original[(col + x + (row + y) * width) * 4 + 2];
                alpha = original[(col + x + (row + y) * width) * 4 + 3];
                weight += ((calcLuminance((red << 24) + (green << 16) + (blue << 8) + alpha) & 0x0000ff00) >> 8) * kernel[x + kernel_size + (y + kernel_size) * kernel_width];
                pixel_count++;
            }
        }
    }

    if (weight < 0){
        weight = 0;
    } else if (weight > 255){
        weight = 255;
    }

    altered[(col + row * width) * 4] = weight;
    altered[(col + row * width) * 4 + 1] = weight;
    altered[(col + row * width) * 4 + 2] = weight;
    altered[(col + row * width) * 4 + 3] = original[(col + row * width) * 4 + 3];
}

unsigned char* edgeDetectionHorizontal(unsigned char* image, struct OperationState state){
    int kernel[] = {1, 2, 1,
                    0, 0, 0,
                    -1, -2, -1};

    switch (state.algo){
        case MULTI_THREAD: break;
        case GPU_ACCELERATED: break;
        default: image = applyKernelSingleThread(image, &detectPixelEdge, kernel, 1, state.width, state.height, 1);
    }

    return image;
}

unsigned char* edgeDetectionVertical(unsigned char* image, struct OperationState state){
    int kernel[] = { -1, 0, 1,
                     -2, 0, 2,
                     -1, 0, 1};

    switch (state.algo){
        case MULTI_THREAD: break;
        case GPU_ACCELERATED: break;
        default: image = applyKernelSingleThread(image, &detectPixelEdge, kernel, 1, state.width, state.height, 1);
    }

    return image;
}

void detectSobelEdge(unsigned char* original, unsigned char* altered, int* NULL_DO_NOT_USE, int kernel_size, int col, int row, int width, int height){
    int horizontal_kernel[] = {1, 2, 1,
                               0, 0, 0,
                               -1, -2, -1};

    int vertical_kernel[] =  {-1, 0, 1,
                              -2, 0, 2,
                              -1, 0, 1};


    int red = 0, green = 0, blue = 0, alpha = 0, pixel_count = 0;
    int horizontal_weight = 0, vertical_weight = 0, combined_weight = 0;
    const int kernel_width = 2 * kernel_size + 1;

    for (int y = -kernel_size; y <= kernel_size; y++){
        for (int x = -kernel_size; x <= kernel_size; x++){
            if (col + x >= 0 && col + x < width && row + y >= 0 && row + y < height){
                red = original[(col + x + (row + y) * width) * 4];
                green = original[(col + x + (row + y) * width) * 4 + 1];
                blue = original[(col + x + (row + y) * width) * 4 + 2];
                alpha = original[(col + x + (row + y) * width) * 4 + 3];
                horizontal_weight += ((calcLuminance((red << 24) + (green << 16) + (blue << 8) + alpha) & 0x0000ff00) >> 8) * horizontal_kernel[x + kernel_size + (y + kernel_size) * kernel_width];
                vertical_weight += ((calcLuminance((red << 24) + (green << 16) + (blue << 8) + alpha) & 0x0000ff00) >> 8) * vertical_kernel[x + kernel_size + (y + kernel_size) * kernel_width];
                pixel_count++;
            }
        }
    }

    combined_weight = sqrt(horizontal_weight * horizontal_weight + vertical_weight * vertical_weight);

    if (combined_weight < 0){
        combined_weight = 0;
    } else if (combined_weight > 255){
        combined_weight = 255;
    }

    altered[(col + row * width) * 4] = combined_weight;
    altered[(col + row * width) * 4 + 1] = combined_weight;
    altered[(col + row * width) * 4 + 2] = combined_weight;
    altered[(col + row * width) * 4 + 3] = original[(col + row * width) * 4 + 3];
}

unsigned char* edgeDetectionSobel(unsigned char* image, struct OperationState state){
    //no kernel defined as uses specific sobel edge detection function

    switch (state.algo){
        case MULTI_THREAD: break;
        case GPU_ACCELERATED: break;
        default: image = applyKernelSingleThread(image, &detectSobelEdge, NULL, 1, state.width, state.height, 1);
    }

    return image;
}
