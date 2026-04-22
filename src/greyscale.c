#include "../inc/greyscale.h"
#include "../inc/types.h"
#include <stdint.h>
#include <string.h>

unsigned char* applyPerPixelSingleChannelSingleThread(unsigned char* image, unsigned int* weighting, int width, int height){
    for (int i = 0; i < width * height * 4; i += 4){
        uint32_t colour = 0;

        colour += (image[i] * weighting[0]);
        colour += (image[i + 1] * weighting[1]);
        colour += (image[i + 2] * weighting[2]);
        colour /= 255;
        colour = colour & 0xff;

        image[i] = colour;
        image[i + 1] = colour;
        image[i + 2] = colour;
    }

    return image;
}

unsigned char* greyscaleLuminance(unsigned char* image, struct OperationState state){

    //hardcoded values of weighting of rgb to human eye from 0-255
    unsigned int weighting[] = {54, 182, 19};

    switch (state.algo){
        case MULTI_THREAD: break;
        case GPU_ACCELERATED: break;
        default: return applyPerPixelSingleChannelSingleThread(image, weighting, state.width, state.height);
    }

    return NULL;
}

unsigned char* applyBrightnessGreyscaleSingleThread(unsigned char* image, int width, int height){
    for (int i = 0; i < width * height * 4; i += 4){
        uint32_t colour = 0;

        colour += image[i] + image[i + 1] + image[i + 2];
        colour /= 3;
        colour &= 0xff;

        image[i] = colour;
        image[i + 1] = colour;
        image[i + 2] = colour;
    }

    return image;
}

unsigned char* greyscaleBrightness(unsigned char* image, struct OperationState state){

    switch (state.algo){
        case MULTI_THREAD: break;
        case GPU_ACCELERATED: break;
        default: return applyBrightnessGreyscaleSingleThread(image, state.width, state.height);
    }

    return NULL;
}

unsigned char* greyscaleRGB(unsigned char* image, struct OperationState state){

    unsigned int weighting[] = {0, 0, 0};
    weighting[state.arg1] = 255;

    switch (state.algo){
        case MULTI_THREAD: break;
        case GPU_ACCELERATED: break;
        default: return applyPerPixelSingleChannelSingleThread(image, weighting, state.width, state.height);
    }

    return NULL;
}
