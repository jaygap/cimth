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
    } else if (strcmp(argv[1], "--mode=gaussian-blur") == 0){
        state.mode = BLUR_GAUSSIAN;
    } else if (strcmp(argv[1], "--mode=luminance-greyscale") == 0){
        state.mode = GREYSCALE_LUMINANCE;
        state.algo = SINGLE_THREAD;
        state.input_file = argv[2];
        state.output_file = argv[3];
        state.include_alpha = 0;
        *status = 0;
        return state;
    }

    if (argc > 5 && (strcmp(argv[2], "--single-threaded") == 0 || strcmp(argv[2], "-s") == 0)){
        state.algo = SINGLE_THREAD;
        state.arg1 = atoi(argv[3]);
        state.arg2 = atoi(argv[3]);
        state.input_file = argv[5];
        state.output_file = argv[6];
        state.include_alpha = 0;
        *status = 0;
    } else{
        printf("Not enough arguments were given. Please see the input formatting with 'cimth --help' or 'cimth -h'\n");
        *status = -1;
        return state;
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
    unsigned char* gaussianBlur(unsigned char*, struct OperationState);
    unsigned char* greyscaleLuminance(unsigned char*, struct OperationState);

    int status;
    struct OperationState state = parseArguments(argc, argv, &status);

    if (status != 0){
        return status;
    }

    //output is rgba
    unsigned char* image = stbi_load(state.input_file, &state.width, &state.height, &state.colour_channels, 4);
    unsigned char* original_pointer = image;

    switch (state.mode){
        case BLUR_BOX: 
            image = boxBlur(image, state);
            break;
        case BLUR_GAUSSIAN:
            image = gaussianBlur(image, state);
            break;
        case GREYSCALE_LUMINANCE:
            image = greyscaleLuminance(image, state);
            break;
        default: break;
    }

    if (image != NULL){
        stbi_write_png(state.output_file, state.width, state.height, 4, image, state.width * 4);
    }

    stbi_image_free(original_pointer);

    return 0;
}

unsigned char* applyKernelToImageSingleThread(unsigned char* original, int* kernel, int kernel_size, int width, int height, int passes){
    void applyKernelToPixel(unsigned char* original, unsigned char* altered, int* kernel, int kernel_size, int col, int row, int width, int height);

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


void applyPerPixelOperation(unsigned char* image, unsigned int* weighting, int index){
    uint32_t colour = 0;

    colour += (image[index] * weighting[0]);
    colour += (image[index + 1] * weighting[1]);
    colour += (image[index + 2] * weighting[2]);
    colour /= 255;
    colour = colour & 0xff;

    image[index] = colour;
    image[index + 1] = colour;
    image[index + 2] = colour;
}

unsigned char* applyPerPixelSingleThread(unsigned char* image, unsigned int* weighting, int width, int height){
    for (int i = 0; i < width * height * 4; i += 4){
        applyPerPixelOperation(image, weighting, i);
    }

    return image;
}

unsigned char* greyscaleLuminance(unsigned char* image, struct OperationState state){

    //hardcoded values of luminance from 0-255
    unsigned int weighting[] = {54, 182,19};

    switch (state.algo){
        case MULTI_THREAD: break;
        case GPU_ACCELERATED: break;
        default: return applyPerPixelSingleThread(image, weighting, state.width, state.height);
    }

    return NULL;
}
