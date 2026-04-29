#include "./types.h"
#include <stdint.h>

//general
// define applyKernelSingleThread

//blur methods
unsigned char* boxBlur(unsigned char*, struct OperationState);
unsigned char* gaussianBlur(unsigned char*, struct OperationState);

unsigned char* applyBlurSingleThread(unsigned char*, int*, int, int, int, int);
void blurPixel(unsigned char*, unsigned char*, int*, int, int, int, int, int);

//edge-detection methods
unsigned char* edgeDetectionHorizontal(unsigned char*, struct OperationState);

void detectPixelEdge(unsigned char*, unsigned char*, int*, int, int, int ,int ,int);
unsigned char* detectEdgesSingleThread(unsigned char*, uint32_t (*)(uint32_t), int*, int, int, int, int, int);
