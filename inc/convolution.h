#include "./types.h"
#include <stdint.h>

//general
unsigned char* applyKernelSingleThread(unsigned char*, void (*)(unsigned char*, unsigned char*, int*, int, int, int, int, int), int*, int, int, int, int);

//blur methods
unsigned char* boxBlur(unsigned char*, struct OperationState);
unsigned char* gaussianBlur(unsigned char*, struct OperationState);

void blurPixel(unsigned char*, unsigned char*, int*, int, int, int, int, int);

//edge-detection methods
unsigned char* edgeDetectionHorizontal(unsigned char*, struct OperationState);
unsigned char* edgeDetectionVertical(unsigned char*, struct OperationState);
unsigned char* edgeDetectionSobel(unsigned char*, struct OperationState);
unsigned char* edgeDetectionCanny(unsigned char*, struct OperationState);

void detectEdgeCanny(unsigned char*, unsigned char*, int*, int, int, int, int, int);
void detectSobelEdge(unsigned char*, unsigned char*, int*, int, int, int, int, int);
void detectPixelEdge(unsigned char*, unsigned char*, int*, int, int, int, int ,int);
unsigned char* detectEdgesSingleThread(unsigned char*, uint32_t (*)(uint32_t), int*, int, int, int, int, int);
