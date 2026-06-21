#include "../inc/types.h"
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>

void swap(uint32_t* a, uint32_t* b){
    uint32_t temp = *a;
    *a = *b;
    *b = temp;
}

int partition(uint32_t* array, uint32_t (*func)(uint32_t, uint32_t), int low, int high){
    int pivot = array[high];

    int i = low - 1;

    for (int j = low; j <= high; j++){
        if (func(array[j], pivot)){
            i++;
            swap(&array[i], &array[j]);
        }
    }

    return i + 1;
}

//add opposite sorting option
void quickSortSingleThread(uint32_t* pixels, uint32_t (*func)(uint32_t, uint32_t), int low, int high){

    if (low < high){

        int pivot = partition(pixels, func, low, high);

        quickSortSingleThread(pixels, func, low, pivot - 1);
        quickSortSingleThread(pixels, func, pivot + 1, high);
    }
}

unsigned char* sortHorizontalSingleThread(unsigned char* image, uint32_t (*func)(uint32_t, uint32_t), struct OperationState state){
    //split pixel rows into arrays, sort, then copy result back into image

    uint32_t* row = (uint32_t*)malloc(state.width);

    for(int y = 0; y < state.height; y++){
        for(int x = 0; x < state.width; x++){
            int offset = (y * state.width + x) * 4;
            row[x] = (image[offset] << 24) + (image[offset + 1] << 16) + (image[offset + 2] << 8) + image[offset + 3];
        }

        quickSortSingleThread(row, func, 0, state.width * 4 - 1);

        for(int x = 0; x < state.width; x++){
            int offset = (y * state.width + x) * 4;
            image[offset] = (row[x] & 0xff000000) >> 24;
            image[offset + 1] = (row[x] & 0x00ff0000) >> 16;
            image[offset + 2] = (row[x] & 0x0000ff00) >> 8;
            image[offset + 3] = row[x] & 0x000000ff;
        }
    }

    free(row);

    return image;
}

uint32_t redCompareAscending(uint32_t a, uint32_t b){
    if ((a & 0xff000000) > (b & 0xff000000)){
        return a;
    } else{
        return b;
    }
}

unsigned char* sortHorizontal(unsigned char* image, struct OperationState state){
    //implement properly later

    switch (state.algo){
        case GPU_ACCELERATED: break;
        case MULTI_THREAD: break;
        case SINGLE_THREAD: return sortHorizontalSingleThread(image, &redCompareAscending, state);
    }

    return NULL;
}
