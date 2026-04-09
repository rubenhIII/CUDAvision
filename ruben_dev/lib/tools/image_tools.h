#ifndef IMAGE_TOOLS_H
#define IMAGE_TOOLS_H

#include <stdlib.h>
#include <stdio.h>

unsigned char** image_zeros(int width, int heigh);
unsigned char** image_copy(unsigned char **image, int width, int height);
void image_free(unsigned char **image, int width, int height);
void image_print(unsigned char **image, int width, int height);
unsigned char** image_to_gray(unsigned char **r, unsigned char **g, unsigned char **b, int width, int height);
unsigned char** image_rgb_channel(unsigned char *rgb_vector, int vec_size, int width, int height, const char channel);
unsigned char** image_rgb_to_vector(unsigned char **r, unsigned char **g, unsigned char **b, int width, int height);

#endif