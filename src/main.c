#include <stdio.h>
#include <string.h>
#define STB_IMAGE_IMPLEMENTATION
#include "../inc/stb_image.h"

// TODO
//
// argument parser
// implement one algorithm single-threaded, then multi-threaded, then hardware accelerated
// repeat for rest

enum Mode{
    BLUR_BOX,
    BLUR_GAUSSIAN, //have each type of function as separate things
    EDGE_DETECTION_HORIZONTAL,
    EDGE_DETECTION_VERTICAL,
    EDGE_DETECTION_SOBEL,
    EDGE_DETECTION_CANNY,
    EDGE_DETECTION_KOVALEVSKY,
    GREYSCALE_LUMINANCE,
    GREYSCALE_BRIGHTNESS,
    GREYSCALE_RGB,
    MASK_LUMINANCE,
    MASK_BRIGHTNESS,
    MASK_RGB,
    PIXEL_SORT_HORIZONTAL,
    PIXEL_SORT_VERTICAL,
    RESIZE_NEAREST_NEIGHBOUR,
    RESIZE_BOX_SAMPLING,
    RESIZE_BILINEAR,
    RESIZE_BICUBIC,
    CROP
};

enum AlgoType{
    SINGLE_THREAD,
    MULTI_THREAD,
    GPU_ACCELERATED
};

struct OperationState{
    enum Mode mode;
    enum AlgoType algo;
    int arg1;
    int arg2;
    int arg3;
};

struct OperationState parseArguments(int argc, char** argv, int* status, int* options){
    struct OperationState state;

    if (argc <= 1){
        printf("Expected arguments but none were given. Try cimth --help.\n");
        *status = -1;
        return state;
    }

    if (strcmp(argv[0], "--help") == 1 || strcmp(argv[0], "-h") == 1){
        printf("Usage: cimth [MODE]... [OPTIONS]... [INPUT-FILE(S)] [OUTPUT-FILE]\nPerform an operation dictated by [MODE] on [INPUT-FILE] and save the output to [OUTPUT-FILE]\n");
        //TODO COMPLETE --HELP
        (*status) = 1;
        return state;
    }

    return state;
}


int main(int argc, char **argv){
    int status;
    int option_count = 0;
    struct OperationState state = parseArguments(argc, argv, &status, &option_count);

    if (status != 0){
        return status;
    }

    int width, height, colour_channels;
    unsigned char* image = stbi_load(argv[2], &width, &height, &colour_channels, 3);

    return 0;
}
