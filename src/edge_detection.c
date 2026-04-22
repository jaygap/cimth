#include "../inc/types.h"
#include "../inc/convolution.h"
#include <string.h>

unsigned char* edgeDetectionHorizontal(unsigned char* image, struct OperationState state){
    int kernel[] = {-1, -1, -1, 0, 0, 0, 1, 1, 1};

    switch (state.algo){
        case MULTI_THREAD: break;
        case GPU_ACCELERATED: break;
        default: return applyKernelToImageSingleThread(image, kernel, 1, state.width, state.height, 0);
    }

    return NULL;
}
