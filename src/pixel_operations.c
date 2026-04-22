#include <stdint.h>
#include "../inc/pixel_operations.h"

unsigned char calcLuminance(unsigned char red, unsigned char green, unsigned char blue){
    unsigned int weighting[] = {54, 182,19};
    uint32_t colour = 0;

    colour += red * weighting[0];
    colour += green * weighting[1];
    colour += blue * weighting[2];
    colour /= 255;

    return colour & 0xff;
}

unsigned char calcBrightness(unsigned char red, unsigned char green, unsigned char blue){
    return ((red + green + blue) / 3) & 0xff;
}

unsigned char getRed(unsigned char red, unsigned char green, unsigned char blue){
    return red;
}

unsigned char getGreen(unsigned char red, unsigned char green, unsigned char blue){
    return green;
}

unsigned char getBlue(unsigned char red, unsigned char green, unsigned char blue){
    return blue;
}
