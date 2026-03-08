#include <limits.h>
#include <stdio.h>
#include <string.h>
#define STB_IMAGE_IMPLEMENTATION
#include "../inc/stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "../inc/stb_image_write.h"

// TODO
//
// argument parser
// implement all algorithms as single-threaded, then multi-threaded, then gpu
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
    char* input_file;
    char* output_file;
    int include_alpha;
    int width;
    int height;
    int colour_channels;
    int arg1;
    int arg2;
    int arg3;
};

struct OperationState parseArguments(int argc, char** argv, int* status){
    struct OperationState state;
    *status = 0;

    if (argc <= 1){
        printf("Expected arguments but none were given. Try cimth --help.\n");
        *status = -1;
        return state;
    }

    if (strcmp(argv[1], "--help") == 0 || strcmp(argv[1], "-h") == 0){
        printf("Usage: cimth [MODE]... [OPTIONS]... [INPUT-FILE(S)] [OUTPUT-FILE]\nPerform an operation dictated by [MODE] on [INPUT-FILE] and save the output to [OUTPUT-FILE]\n");
        //TODO COMPLETE --HELP
        *status = 1;
        return state;
    } else if (strcmp(argv[1], "--mode=box-blur") == 0){
        state.mode = BLUR_BOX;

        if (argc > 5 && (strcmp(argv[2], "--single-threaded") == 0 || strcmp(argv[2], "-s") == 0)){
            state.algo = SINGLE_THREAD;
            state.arg1 = atoi(argv[3]);
            state.arg2 = 1;
            state.input_file = argv[4];
            state.output_file = argv[5];
            state.include_alpha = 0;
            *status = 0;
        } else{
            printf("Not enough arguments were given. Please see the input formatting with 'cimth --help' or 'cimth -h'\n");
            *status = -1;
            return state;
        }
    }

    return state;
}

void testPrintPixelData(unsigned char* image, struct OperationState* state){
    int counter = 0;

    for (int i = 0; i < state->width * state->height * 4; i++){
        printf("%d ", image[i]);
        counter += 1;

        if (counter == 4){
            printf("\n");
            counter = 0;
        }
    }
}

int main(int argc, char **argv){
    //function declaration
    unsigned char* boxBlur(unsigned char*, struct OperationState);

    int status;
    struct OperationState state = parseArguments(argc, argv, &status);

    if (status != 0){
        return status;
    }

    //output is rgba
    unsigned char* image = stbi_load(state.input_file, &state.width, &state.height, &state.colour_channels, 4);
    unsigned char* original_pointer = image;

    image = boxBlur(image, state);

    if (image != NULL){
        stbi_write_png(state.output_file, state.width, state.height, 4, image, state.width * 4);
    }

    stbi_image_free(original_pointer);

    return 0;
}


unsigned char* boxBlur(unsigned char* image, struct OperationState state){
    //arg1 of OperationState is kernel_size and arg2 is number of passes to perform

    unsigned char* boxBlurSingleThread(unsigned char* image, struct OperationState* state);

    switch (state.algo){
        case MULTI_THREAD: break; //IMPLEMENT THIS
        case GPU_ACCELERATED: break; //IMPLEMENT THIS
        default: return boxBlurSingleThread(image, &state);
    };

    return NULL;
}

void boxBlurPixel(unsigned char* original, unsigned char* blurred, int col, int row, int width, int height, int kernel_size){
    unsigned int red = 0, green = 0, blue = 0, alpha = 0, pixel_count = 0;

    for (int y = -kernel_size; y <= kernel_size; y++){
        for (int x = -kernel_size; x <= kernel_size; x++){
            if (col + x >= 0 && col + x < width && row + y >= 0 && row + y < height){
                red += original[(col + x + (row + y) * width) * 4];
                green += original[(col + x + (row + y) * width) * 4 + 1];
                blue += original[(col + x + (row + y) * width) * 4 + 2];
                alpha += original[(col + x + (row + y) * width) * 4 + 3];
                pixel_count++;
            }
        }
    }

    red /= pixel_count;
    green /= pixel_count;
    blue /= pixel_count;
    alpha /= pixel_count;

    blurred[(col + row * width) * 4] = red;
    blurred[(col + row * width) * 4 + 1] = green;
    blurred[(col + row * width) * 4 + 2] = blue;
    blurred[(col + row * width) * 4 + 3] = alpha;
}

unsigned char* boxBlurSingleThread(unsigned char* image, struct OperationState* state){
    int width = state->width;
    int height = state->height;
    int kernel = state->arg1;
    int passes = state->arg2;

    unsigned char* blurred = (unsigned char*)malloc(width * height * 4);

    if (!blurred){
        printf("Failed to allocate memory for image in heap.");
        return NULL;
    }

    for (int p = 0; p < passes; p++){
        for (int y = 0; y < height; y++){
            for (int x = 0; x < width; x++){
                if (p % 2 == 0){
                    boxBlurPixel(image, blurred, x, y, width, height, kernel);
                } else{
                    boxBlurPixel(blurred, image, x, y, width, height, kernel);
                }
            }
        }
    }

    if (passes % 2 == 1){
        memcpy(image, blurred, width * height * 4);
    }

    free(blurred);

    return image;
}
