#include "./types.h"
#include <stdint.h>

unsigned char* applyMaskFunctionSingleThreaded(unsigned char*, uint32_t (*)(uint32_t), int, int, int);
unsigned char calcOtsuThresholdSingleThread(unsigned char*, uint32_t (*)(uint32_t), int, int);
unsigned char calcOtsuThreshold(unsigned char*, uint32_t (*)(uint32_t), struct OperationState);
unsigned char* maskLuminance(unsigned char*, struct OperationState);
unsigned char* maskBrightness(unsigned char*, struct OperationState);
unsigned char* maskRGB(unsigned char*, struct OperationState);

