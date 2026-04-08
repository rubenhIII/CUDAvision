#include "image_tools.h"

void image_init(unsigned char **image, int width, int height)
{
    image = (unsigned char*) malloc (sizeof(unsigned char*) * height);
    for (int i = 0; i < width; i++) {
        image[i] = (unsigned char) malloc (sizeof(unsigned char) * width);
    }
    image_zero(image, width, height);
}

void image_zero(unsigned char **image, int width, int heigh)
{
    for (int i = 0; i < heigh; i++) {
        for (int j = 0; j < width; j++) {
            image[i][j] = 0;
        }
    }
}