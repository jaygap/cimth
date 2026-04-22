#include "./types.h"

//generic convolution methods
void applyKernelToPixel(unsigned char*, unsigned char*, int*, int, int, int, int, int);
unsigned char* applyKernelToImageSingleThread(unsigned char*, int*, int, int, int, int);

//blur methods
unsigned char* boxBlur(unsigned char*, struct OperationState);
unsigned char* gaussianBlur(unsigned char*, struct OperationState);
