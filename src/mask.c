#include "../inc/types.h"
#include "../inc/mask.h"
#include "../inc/pixel_operations.h"
#include <string.h>

unsigned char* applyMaskFunctionSingleThreaded(unsigned char* image, unsigned char (*maskFunction)(unsigned char, unsigned char, unsigned char), int threshold, int width, int height){
    for (int x = 0; x < width * height * 4; x += 4){
        unsigned char colour;
        if (maskFunction(image[x], image[x + 1], image [x + 2]) >= threshold){
            colour = 0;
        } else{
            colour = 255;
        }

        image[x] = colour;
        image[x + 1] = colour;
        image[x + 2] = colour;
    }

    return image;
}

unsigned char calcOtsuThresholdSingleThread(unsigned char* image, unsigned char (*getPixelProperty)(unsigned char, unsigned char, unsigned char), int width, int height){
    const int range = 256;
    int histogram[range];
    double highest_variance = -1.0;
    int highest_variance_index = 0;

    int total_pixels = width * height;
    long long total_weighted_sum = 0;
    int weight_background = 0;
    long long sum_background = 0;

    for (int i = 0; i < range; i++){
        histogram[i] = 0;
    }

    for (int row = 0; row < height; row++){
        for (int col = 0; col < width; col++){
            int index = (row * width + col) * 4;
            unsigned char value = getPixelProperty(image[index], image[index + 1], image[index + 2]);
            histogram[value]++;
        }
    }

    for (int i = 0; i < range; i++){
        total_weighted_sum += (long long)i * histogram[i];
    }

    for (int i = 0; i < range; i++){
        weight_background += histogram[i];
        if (weight_background == 0){
            continue;
        }

        int weight_foreground = total_pixels - weight_background;
        if (weight_foreground == 0){
            break;
        }

        sum_background += (long long)i * histogram[i];

        double mu_background = (double)sum_background / weight_background;
        double mu_foreground = (double)(total_weighted_sum - sum_background) / weight_foreground;
        double mean_diff = mu_background - mu_foreground;
        double square_variance = (double)weight_background * weight_foreground * mean_diff * mean_diff;

        if (square_variance > highest_variance){
            highest_variance = square_variance;
            highest_variance_index = i;
        }
    }

    return highest_variance_index;
}

unsigned char calcOtsuThreshold(unsigned char* image, unsigned char (*func)(unsigned char, unsigned char, unsigned char), struct OperationState state){
    switch (state.algo){
        case MULTI_THREAD: break;
        case GPU_ACCELERATED: break;
        default: return calcOtsuThresholdSingleThread(image, func, state.width, state.height);
    }

    return 0;
}

unsigned char* maskLuminance(unsigned char* image, struct OperationState state){
    int threshold;

    if (state.arg1 == -1){
        threshold = calcOtsuThreshold(image, &calcLuminance, state);
    } else{
        threshold = state.arg1;
    }

    switch (state.algo){
        case MULTI_THREAD: break;
        case GPU_ACCELERATED: break;
        default: return applyMaskFunctionSingleThreaded(image, &calcLuminance, threshold, state.width, state.height);
    }

    return NULL;
}

unsigned char* maskBrightness(unsigned char* image, struct OperationState state){
    int threshold;

    if (state.arg1 == -1){
        threshold = calcOtsuThreshold(image, &calcBrightness, state);
    } else{
        threshold = state.arg1;
    }

    switch (state.algo){
        case MULTI_THREAD: break;
        case GPU_ACCELERATED: break;
        default: return applyMaskFunctionSingleThreaded(image, &calcBrightness, threshold, state.width, state.height);
    }

    return NULL;
}

unsigned char* maskRGB(unsigned char* image, struct OperationState state){
    int threshold;

    unsigned char (*func)(unsigned char, unsigned char, unsigned char);

    if (state.arg2 == 0){
        func = &getRed;
    } else if (state.arg2 == 1){
        func = &getGreen;
    } else{
        func = &getBlue;
    }

    if (state.arg1 == -1){
        threshold = calcOtsuThreshold(image, func, state);
    } else{
        threshold = state.arg1;
    }

    switch (state.algo){
        case MULTI_THREAD: break;
        case GPU_ACCELERATED: break;
        default: return applyMaskFunctionSingleThreaded(image, func, threshold, state.width, state.height);
    }

    return NULL;
}
