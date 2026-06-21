#include "../inc/types.h"
#include <stdint.h>

unsigned char* sortHorizontal(unsigned char* ,struct OperationState state);

unsigned char* sortHorizontalSingleThread(unsigned char* image, uint32_t (*func)(uint32_t), struct OperationState state);
unsigned char* sortVerticalSingleThread(unsigned char* image, uint32_t (*func)(uint32_t), struct OperationState state);

unsigned char* sortHorizontalMaskSingleThread(unsigned char* image, unsigned char* mask, uint32_t (*func)(uint32_t), struct OperationState state);
unsigned char* sortVerticalMaskSingleThread(unsigned char* image, unsigned char* mask, uint32_t (*func)(uint32_t), struct OperationState state);

//methods used to compare pixels
