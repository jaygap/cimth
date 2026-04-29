#include <stdint.h>
#include "../inc/pixel_operations.h"

uint32_t calcLuminance(uint32_t colour){
    unsigned int weighting[] = {54, 182,19};
    uint32_t luminance = 0;

    luminance += ((colour & 0xff000000) >> 24) * weighting[0];
    luminance += ((colour & 0x00ff0000) >> 16) * weighting[1];
    luminance += ((colour & 0x0000ff00) >> 8) * weighting[2];
    luminance = (luminance / 255) & 0xff;

    return (luminance << 24) + (luminance << 16) + (luminance << 8) + (colour & 0xff);
}

uint32_t calcBrightness(uint32_t colour){
    uint32_t brightness = 0;

    brightness += (colour & 0xff000000) >> 24;
    brightness += (colour & 0x00ff0000) >> 16;
    brightness += (colour & 0x0000ff00) >> 8;
    brightness = (brightness / 3) & 0xff;

    return (brightness << 24) + (brightness << 16) + (brightness << 8) + (colour & 0xff);
}

uint32_t getRed(uint32_t colour){
    return (colour & 0xff000000) >> 24;
}

uint32_t getGreen(uint32_t colour){
    return (colour & 0x00ff0000) >> 16;
}

uint32_t getBlue(uint32_t colour){
    return (colour & 0x0000ff00) >> 8;
}

uint32_t getAlpha(uint32_t colour){
    return (colour & 0x000000ff);
}
