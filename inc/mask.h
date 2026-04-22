#include "./types.h"

unsigned char* applyMaskFunctionSingleThreaded(unsigned char*, unsigned char (*)(unsigned char, unsigned char, unsigned char), int, int, int);
unsigned char calcOtsuThresholdSingleThread(unsigned char*, unsigned char (*)(unsigned char, unsigned char, unsigned char), int, int);
unsigned char calcOtsuThreshold(unsigned char*, unsigned char (*)(unsigned char, unsigned char, unsigned char), struct OperationState);
unsigned char* maskLuminance(unsigned char*, struct OperationState);
unsigned char* maskBrightness(unsigned char*, struct OperationState);
unsigned char* maskRGB(unsigned char*, struct OperationState);

