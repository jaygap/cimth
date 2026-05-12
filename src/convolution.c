//convolutions include blurs and edge-detection algorithms

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
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

unsigned char* detectCannyEdgeSingleThread(unsigned char* image, struct OperationState state){
    // 1. gaussian blur
    // 2. find intensity of gradients
    // 3. apply gradient magnitude thresholding or lower bound cut-off suppression to get rid of spurious response to edge detection
    // 4. apply double threshold to determine potential edges
    // 5. track edge by hysteresis

    int width = state.width;
    int height = state.height;

    // 1. gaussian blur
    int kernel[] = {1, 2, 1, 2, 4, 2, 1, 2, 1};
    const int kernel_size = 1;
    image = applyKernelSingleThread(image, &blurPixel, kernel, kernel_size, width, height, 1);

    // 2. find intensity of gradients
    int* intensities = (int*)malloc(width * height * sizeof(int));
    int* gradients = (int*)malloc(width * height * sizeof(int));

    int horizontal_kernel[] = {1, 2, 1,
                               0, 0, 0,
                               -1, -2, -1};

    int vertical_kernel[] =  {-1, 0, 1,
                              -2, 0, 2,
                              -1, 0, 1};


    int red = 0, green = 0, blue = 0, alpha = 0, pixel_count = 0;
    int horizontal_weight = 0, vertical_weight = 0, combined_weight = 0;
    const int kernel_width = 2 * kernel_size + 1;

    //fix this at some point
    for (int row = 0; row < height; row++){
        for (int col = 0; col < width; col++){
            horizontal_weight = 0;
            vertical_weight = 0;
            for (int y = -kernel_size; y <= kernel_size; y++){
                for (int x = -kernel_size; x <= kernel_size; x++){
                    if (col + x >= 0 && col + x < width && row + y >= 0 && row + y < height){
                        red = image[(col + x + (row + y) * width) * 4];
                        green = image[(col + x + (row + y) * width) * 4 + 1];
                        blue = image[(col + x + (row + y) * width) * 4 + 2];
                        alpha = image[(col + x + (row + y) * width) * 4 + 3];
                        horizontal_weight += ((calcLuminance((red << 24) + (green << 16) + (blue << 8) + alpha) & 0x0000ff00) >> 8) * horizontal_kernel[x + kernel_size + (y + kernel_size) * kernel_width];
                        vertical_weight += ((calcLuminance((red << 24) + (green << 16) + (blue << 8) + alpha) & 0x0000ff00) >> 8) * vertical_kernel[x + kernel_size + (y + kernel_size) * kernel_width];

                        double intensity = atan2(vertical_weight, horizontal_weight);

                        if (intensity < 0){
                            intensity = -intensity;
                        }

                        intensity = intensity * (180.0 / 3.141592);


                        if ((intensity >= 0 && intensity < 23) || (intensity >= 158 && intensity < 181)){
                            intensity = 0;
                        } else if (intensity >= 23 && intensity < 68){
                            intensity = 45;
                        } else if (intensity >= 68 && intensity < 113){
                            intensity = 90;
                        } else if (intensity >= 113 && intensity < 158){
                            intensity = 135;
                        } else{
                           printf("INVALID INTENSITY VALUE: %lf\n", intensity);
                        }

                        intensities[col + row * width] = intensity;
                        gradients[col + row * width] = sqrt(horizontal_weight * horizontal_weight + vertical_weight * vertical_weight);
                    }
                }
            }
        }
    }

    // 3. gradient magnitude thresholding or lower bound cut off suppression
    int neighbours_to_check[2];

    for (int i = 0; i < width * height; i++){
        bool suppress = false;

        if (intensities[i] == 0){
            neighbours_to_check[0] = -1;
            neighbours_to_check[1] = 1;
        } else if (intensities[i] == 45){
            neighbours_to_check[0] = -width + 1;
            neighbours_to_check[1] = width - 1;
        } else if (intensities[i] == 90){
            neighbours_to_check[0] = -width;
            neighbours_to_check[1] = width;
        } else if (intensities[i] == 135){
            neighbours_to_check[0] = -width - 1;
            neighbours_to_check[1] = width + 1;
        }

        if (i + neighbours_to_check[0] > 0){
            if (gradients[i + neighbours_to_check[0]] > gradients[i]){
                suppress = true;
            }
        }
        if (i + neighbours_to_check[1] < width * height){
            if (gradients[i + neighbours_to_check[1]] > gradients[i]){
                suppress = true;
            }
        }

        if (suppress){
            gradients[i] = 0;
        }
    }

    // 4. double thresholding
    unsigned char otsu_threshold = calcOtsuThresholdSingleThread(image, &calcLuminance, state.width, state.height);

    int lower_threshold = 255 / 4;
    int upper_threshold = 255;

    if (lower_threshold * 3 > 255){
        upper_threshold = 255;
    }

    // 5. hysteresis
    for (int i = 0; i < width * height; i++){
        bool real_edge = false;
        if (gradients[i] <= lower_threshold){
            real_edge = false;
        } else if (gradients[i] > lower_threshold && gradients[i] <= upper_threshold){
            for (int x = -width; x <= width; x += width){
                if (!(i + x - 1 > 0 && i + x + 1 < width * height)){
                    continue;
                }

                if (gradients[i + x - 1] > upper_threshold || gradients[i + x] > upper_threshold || gradients[i + x + 1] > upper_threshold){
                    real_edge = true;
                } else {
                    real_edge = false;
                }
            }
        } else {
            real_edge = true;
        }

        if (real_edge){
            image[i * 4] = 255;
            image[i * 4 + 1] = 255;
            image[i * 4 + 2] = 255;
            image[i * 4 + 3] = 255;
        } else{
            image[i * 4] = 0;
            image[i * 4 + 1] = 0;
            image[i * 4 + 2] = 0;
            image[i * 4 + 3] = 255;
        }
    }

    return image;
}

unsigned char* edgeDetectionCanny(unsigned char* image, struct OperationState state){

    switch (state.algo){
        case MULTI_THREAD: break;
        case GPU_ACCELERATED: break;
        default: image = detectCannyEdgeSingleThread(image, state);
    }

    return image;
}
