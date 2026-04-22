#include "./types.h"

unsigned char* greyscaleLuminance(unsigned char*, struct OperationState);
unsigned char* greyscaleBrightness(unsigned char*, struct OperationState);
unsigned char* greyscaleRGB(unsigned char*, struct OperationState);

unsigned char* applyPerPixelSingleChannelSingleThread(unsigned char*, unsigned int*, int, int);
unsigned char* applyBrightnessGreyscaleSingleThread(unsigned char*, int, int);

