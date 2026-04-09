#include "image_tools.h"


unsigned char** image_zeros(int width, int height)
{
    unsigned char** image = (unsigned char **) malloc (sizeof(unsigned char *) * height);
    for (int i = 0; i < height; i++) {
        image[i] = (unsigned char*) malloc (sizeof(unsigned char) * width);
        for (int j = 0; j < width; j++) {
            image[i][j] = 0;
        }
    }
    return image;
}

unsigned char** image_copy(unsigned char **image, int width, int height)
{
    unsigned char **image_cpy = image_zeros(width, height);
    for (int i = 0; i < height; i++)
        for (int j = 0; j < width; j++)
            image_cpy[i][j] = image[i][j];
    return image_cpy;
}

void image_free(unsigned char **image, int width, int height)
{
    for (int i = 0; i < height; i++) free(image[i]);
    free(image);
}

void image_print(unsigned char **image, int width, int height)
{
    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            printf("%u ", (unsigned int) image[i][j]);
        }
        printf("\n");
    }
}

unsigned char** image_to_gray(unsigned char **r, unsigned char **g, unsigned char **b, int width, int height)
{
    // Grayscale Luminosity formula
    // gray = 0.21 * R + 0.72 * G + 0.07 * B
    unsigned char **image_gray = image_zeros(width, height);
    for (int i = 0; i < height; i++)
        for (int j = 0; j < width; j++)
            image_gray[i][j] = 0.21 * r[i][j] + 0.72 * g[i][j] + 0.07 * b[i][j];
    return image_gray;
}

unsigned char** image_rgb_channel(unsigned char *rgb_vector, int vec_size, int width, int height, const char channel)
{
    if (vec_size / 3 != width * height) {
        perror("Error image_rgb_channel: RGB vector size and height x width mismatch");
        return NULL;
    }
    unsigned char **image_channel = image_zeros(width, height);
    int index = 0;
    if (channel == 'r') index = 0;
    else if (channel == 'g') index = 1;
    else if (channel == 'b') index = 2;
    else {
        perror("Error image_rgb_channel: Channel not valid.");
        return NULL;
    }

    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            image_channel[i][j] = rgb_vector[(i * 3 * height) + j * 3 + index];
        }
    }

    return image_channel;
}

unsigned char* image_rgb_to_vector(unsigned char **r, unsigned char **g, unsigned char **b, int width, int height)
{
    unsigned char *rgb_vector = (unsigned char *) malloc (sizeof(unsigned char) * 3 * width, height);

    return rgb_vector;

}