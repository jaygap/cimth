#include "../inc/types.h"
#include "../inc/generic.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

struct OperationState parseArgs(int argc, char** argv, int* status){
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
    } else if (strcmp(argv[1], "--mode=brightness-greyscale") == 0){
        state.mode = GREYSCALE_BRIGHTNESS;
        state.algo = SINGLE_THREAD;
        state.input_file = argv[2];
        state.output_file = argv[3];
        state.include_alpha = 0;
        *status = 0;
        return state;
    } else if (strcmp(argv[1], "--mode=red-greyscale") == 0){
        state.mode = GREYSCALE_RGB;
        state.algo = SINGLE_THREAD;
        state.arg1 = 0;
        state.input_file = argv[2];
        state.output_file = argv[3];
        state.include_alpha = 0;
        *status = 0;
        return state;
    } else if (strcmp(argv[1], "--mode=blue-greyscale") == 0){
        state.mode = GREYSCALE_RGB;
        state.algo = SINGLE_THREAD;
        state.arg1 = 2;
        state.input_file = argv[2];
        state.output_file = argv[3];
        state.include_alpha = 0;
        *status = 0;
        return state;
    } else if (strcmp(argv[1], "--mode=green-greyscale") == 0){
        state.mode = GREYSCALE_RGB;
        state.algo = SINGLE_THREAD;
        state.arg1 = 1;
        state.input_file = argv[2];
        state.output_file = argv[3];
        state.include_alpha = 0;
        *status = 0;
        return state;
    } else if (strcmp(argv[1], "--mode=luminance-mask") == 0){
        state.mode = MASK_LUMINANCE;
        state.algo = SINGLE_THREAD;
        state.input_file = argv[2];
        state.output_file = argv[3];
        state.arg1 = -1;
        state.include_alpha = 1;
        *status = 0;
        return state;
    } else if (strcmp(argv[1], "--mode=brightness-mask") == 0){
        state.mode = MASK_BRIGHTNESS;
        state.algo = SINGLE_THREAD;
        state.input_file = argv[2];
        state.output_file = argv[3];
        state.arg1 = -1;
        state.include_alpha = 1;
        *status = 0;
        return state;
    } else if (strcmp(argv[1], "--mode=red-mask") == 0){
        state.mode = MASK_RGB;
        state.algo = SINGLE_THREAD;
        state.arg1 = -1;
        state.arg2 = 0;
        state.input_file = argv[2];
        state.output_file = argv[3];
        state.include_alpha = 1;
        *status = 0;
        return state;
    } else if (strcmp(argv[1], "--mode=green-mask") == 0){
        state.mode = MASK_RGB;
        state.algo = SINGLE_THREAD;
        state.arg1 = -1;
        state.arg2 = 1;
        state.input_file = argv[2];
        state.output_file = argv[3];
        state.include_alpha = 1;
        *status = 0;
        return state;
    } else if (strcmp(argv[1], "--mode=blue-mask") == 0){
        state.mode = MASK_RGB;
        state.algo = SINGLE_THREAD;
        state.arg1 = -1;
        state.arg2 = 2;
        state.input_file = argv[2];
        state.output_file = argv[3];
        state.include_alpha = 1;
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
