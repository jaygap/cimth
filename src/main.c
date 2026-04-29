#include <limits.h>
#define STB_IMAGE_IMPLEMENTATION
#include "../inc/stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "../inc/stb_image_write.h"

#include "../inc/types.h"
#include "../inc/generic.h"

// TODO
// generalise methods to take function pointers to pixel_operations, change pixel_operations definition (use uint32_t)
// argument parser
// implement all algorithms as single-threaded, then multi-threaded, then gpu

int main(int argc, char **argv){
    int status;
    struct OperationState state = parseArgs(argc, argv, &status);

    if (status != 0){
        return status;
    }

    //output is rgba
    unsigned char* image = stbi_load(state.input_file, &state.width, &state.height, &state.colour_channels, 4);

    performOperation(image, state);

    if (image != NULL){
        stbi_write_png(state.output_file, state.width, state.height, 4, image, state.width * 4);
    }

    stbi_image_free(image);

    return 0;
}

