#include <limits.h>
#define STB_IMAGE_IMPLEMENTATION
#include "../inc/stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "../inc/stb_image_write.h"

#include "../inc/types.h"
#include "../inc/generic.h"
#include "../inc/pixel_operations.h"
#include "../inc/convolution.h"
#include "../inc/greyscale.h"
#include "../inc/mask.h"

// TODO
// generalise methods to take function pointers to pixel_operations, change pixel_operations definition (use uint32_t)
// argument parser
// implement all algorithms as single-threaded, then multi-threaded, then gpu
// repeat for rest

int main(int argc, char **argv){
    int status;
    struct OperationState state = parseArgs(argc, argv, &status);

    if (status != 0){
        return status;
    }

    //output is rgba
    unsigned char* image = stbi_load(state.input_file, &state.width, &state.height, &state.colour_channels, 4);

    switch (state.mode){
        case BLUR_BOX:
            image = boxBlur(image, state);
            break;
        case BLUR_GAUSSIAN:
            image = gaussianBlur(image, state);
            break;
        case GREYSCALE_LUMINANCE:
            image = greyscaleLuminance(image, state);
            break;
        case GREYSCALE_BRIGHTNESS:
            image = greyscaleBrightness(image, state);
            break;
        case GREYSCALE_RGB:
            image = greyscaleRGB(image, state);
            break;
        case MASK_LUMINANCE:
            image = maskLuminance(image, state);
            break;
        case MASK_BRIGHTNESS:
            image = maskBrightness(image, state);
            break;
        case MASK_RGB:
            image = maskRGB(image, state);
            break;
        default: break;
    }

    if (image != NULL){
        stbi_write_png(state.output_file, state.width, state.height, 4, image, state.width * 4);
    }

    stbi_image_free(image);

    return 0;
}

