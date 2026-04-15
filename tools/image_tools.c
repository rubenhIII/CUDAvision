#include "image_tools.h"
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

//#define STB_IMAGE_WRITE_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
//#include "stb_image_write.h"
#include "stb_image.h"

#include "fenster.h"


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

unsigned char* image_to_gray_vector(unsigned char *image, int width, int height)
{
    // Grayscale Luminosity formula
    // gray = 0.21 * R + 0.72 * G + 0.07 * B
    // Evitando el punto flotante (77 * image[idx + 0] + 150 * image[idx + 1] + 29 * image[idx + 2]) >> 8;
    if (!image) return NULL;
    unsigned char *gray_vector = (unsigned char *) malloc (sizeof(unsigned char) * 3 * width * height);
    if (!gray_vector) return NULL;
    unsigned char gray;
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {

            int idx = (y * width + x) * 3;

            gray = (77 * image[idx + 0] + 150 * image[idx + 1] + 29 * image[idx + 2]) >> 8;

            gray_vector[idx + 0] = gray;
            gray_vector[idx + 1] = gray;
            gray_vector[idx + 2] = gray;
        }
    }
    return gray_vector;
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
            image_channel[i][j] = rgb_vector[(j * 3 * height) + i * 3 + index];
        }
    }

    return image_channel;
}

unsigned char* image_rgb_to_vector(unsigned char **r, unsigned char **g, unsigned char **b, int width, int height)
{
    unsigned char *rgb_vector = (unsigned char *) malloc (sizeof(unsigned char) * 3 * width * height);
    
    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {

            int idx = (j * width + i) * 3;
            rgb_vector[idx + 0] = r[i][j];
            rgb_vector[idx + 1] = g[i][j];
            rgb_vector[idx + 2] = b[i][j];
        }
    }

    return rgb_vector;
}

unsigned char* image_load(const char* filename, int *width, int *height, int *channels)
{
    unsigned char *image = stbi_load(filename, width, height, channels, 0);
    return image;
}
    
void image_show(unsigned char *rgb_vector, int width, int height)
{
    unsigned int r, g, b;
    unsigned int *buf = (unsigned int *) malloc (sizeof(unsigned int) * width * height);
    struct fenster f = { .title = "CUDAvision", .width = width, .height = height, .buf = buf };
    fenster_open(&f);
    while (fenster_loop(&f) == 0) {
        for (int y = 0; y < height; y++) {
            for (int x = 0; x < width; x++) {

                // rgb_vector[(y * width + x) * channels + c]
                int idx = (y * width + x) * 3;

                r = rgb_vector[idx + 0] << 16;
                g = rgb_vector[idx + 1] << 8;
                b = rgb_vector[idx + 2];
                fenster_pixel(&f, x, y) = r | g | b;
            }
        }
    }
    free(buf);
    fenster_close(&f);
}

unsigned int* image_hist(unsigned char *gray_image, int width, int height)
{
    unsigned int *hist = (unsigned int *) malloc (sizeof(unsigned int) * BIT_DEPTH);
    for (int i = 0; i < BIT_DEPTH; i++) hist[i] = 0;
    unsigned char index = 0;

    int min_flag=0, max_flag=0;
    for (int i = 0; i < width * height; i++) {
        index = gray_image[i];
        hist[index]++;
    }

    return hist;
}

float* image_hist_norm(unsigned char *gray_image, int width, int height)
{
    float *hist = (float *) malloc (sizeof(float) * BIT_DEPTH);
    float step = 1.0 / (width * height);
    for (int i = 0; i < BIT_DEPTH; i++) hist[i] = 0;
    unsigned char index = 0;
    for (int i = 0; i < width * height; i++) {
        index = gray_image[i];
        hist[index]+=step;
    }

    return hist;
}

float hist_cdf(float *hist_norm, unsigned int m, unsigned int n)
{
    float cdf = 0;
    for (int i = m; i < n; i++) cdf+=hist_norm[i];
    return cdf;
}

float hist_mean(float *hist_norm, unsigned int m, unsigned int n)
{
    float mean = 0;
    for (int i = m; i < n; i++) {
        mean = mean + i * hist_norm[i];
    }
    return mean;
}

unsigned int otsu_thresholding(float* hist_norm)
{
    float w0, w1, mu0, mu1, var = 0, var_aux = 0;
    unsigned int t_star = 0;
    for (unsigned int t = 0; t < BIT_DEPTH; t++) {
        w0 = hist_cdf(hist_norm, 0, t);
        w1 = hist_cdf(hist_norm, t, BIT_DEPTH);
        mu0 = hist_mean(hist_norm, 0, t) / w0;
        mu1 = hist_mean(hist_norm, t, BIT_DEPTH) / w1;

        float mu_diff = mu0 - mu1;
        var_aux = w0 * w1 * mu_diff * mu_diff;

        //printf("t: %d w0: %.004f w1: %.004f mu0: %.004f m1: %.004f var: %.004f var_aux: %.004f \n", t, w0, w1, mu0, mu1, var, var_aux);

        if (var_aux > var) {
            var = var_aux;
            t_star = t;
        }
    }
    
    return t_star;
}

void image_threshold(unsigned int th, unsigned char*gray_image, int width, int height)
{
    unsigned char value;
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            int idx = (y * width + x) * 3;

            value = gray_image[idx] < th ? 0 : 255;;

            gray_image[idx + 0] = value;
            gray_image[idx + 1] = value;
            gray_image[idx + 2] = value;
        }
    }
}