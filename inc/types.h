#ifndef CIMTH_TYPES
#define CIMTH_TYPES

enum Mode{
    BLUR_BOX,
    BLUR_GAUSSIAN,
    EDGE_DETECTION_HORIZONTAL,
    EDGE_DETECTION_VERTICAL,
    EDGE_DETECTION_SOBEL,
    EDGE_DETECTION_CANNY,
    EDGE_DETECTION_KOVALEVSKY,
    GREYSCALE_LUMINANCE,
    GREYSCALE_BRIGHTNESS,
    GREYSCALE_RGB,
    MASK_LUMINANCE,
    MASK_BRIGHTNESS,
    MASK_RGB,
    PIXEL_SORT_HORIZONTAL,
    PIXEL_SORT_VERTICAL,
    RESIZE_NEAREST_NEIGHBOUR,
    RESIZE_BOX_SAMPLING,
    RESIZE_BILINEAR,
    RESIZE_BICUBIC,
    CROP
};

enum AlgoType{
    SINGLE_THREAD,
    MULTI_THREAD,
    GPU_ACCELERATED
};

struct OperationState{
    enum Mode mode;
    enum AlgoType algo;
    char* input_file;
    char* output_file;
    int include_alpha;
    int width;
    int height;
    int colour_channels;
    int arg1;
    int arg2;
    int arg3;
};

#endif
