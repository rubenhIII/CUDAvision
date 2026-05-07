#include "image_process.h"
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

static int KERNEL_GX[3][3]   = { {-1, 0, 1}, 
                                 {-2, 0, 2}, 
                                 {-1, 0, 1} };
static int KERNEL_GY[3][3]   = { {-1,-2,-1}, 
                                 { 0, 0, 0}, 
                                 { 1, 2, 1} };
static int KERNEL_GAUSS[3][3] = { {1,2,1}, 
                                  {2,4,2}, 
                                  {1,2,1} };

unsigned char* image_padding(unsigned char *image, int width, int height, int pad)
{
    int new_w = width  + 2 * pad;
    int new_h = height + 2 * pad;

    unsigned char *padded = (unsigned char*) calloc(new_w * new_h * 3, sizeof(unsigned char));
    if (!padded) return NULL;

    for (int y = 0; y < height; y++)
        for (int x = 0; x < width; x++)
            for (int c = 0; c < 3; c++)
                padded[((y + pad) * new_w + (x + pad)) * 3 + c] = image[(y * width + x) * 3 + c];

    return padded;
}

unsigned char* sobel_filter(unsigned char *image, int width, int height)
{
    const int pad   = 1;
    const int pad_w = width  + 2 * pad;
    const int pad_h = height + 2 * pad;

    int *padded = (int*) calloc(pad_w * pad_h, sizeof(int));
    if (!padded) return NULL;

    for (int y = 0; y < height; y++)
        for (int x = 0; x < width; x++) {
            int idx = (y * width + x) * 3;
            padded[(y + pad) * pad_w + (x + pad)] =
                (77 * image[idx] + 150 * image[idx+1] + 29 * image[idx+2]) >> 8;
        }

    unsigned char *result = (unsigned char*) malloc(width * height * 3);
    if (!result) { free(padded); return NULL; }

    for (int y = pad; y < pad_h - pad; y++) {
        for (int x = pad; x < pad_w - pad; x++) {

            int gx = 0, gy = 0;
            for (int k = 0; k < 3; k++)
                for (int l = 0; l < 3; l++) {
                    int val = padded[(y - pad + k) * pad_w + (x - pad + l)];
                    gx += KERNEL_GX[k][l] * val;
                    gy += KERNEL_GY[k][l] * val;
                }

            int mag = (int)sqrt((double)(gx*gx + gy*gy));
            if (mag > 255) mag = 255;

            int out_idx = ((y - pad) * width + (x - pad)) * 3;
            result[out_idx + 0] = mag;
            result[out_idx + 1] = mag;
            result[out_idx + 2] = mag;
        }
    }

    free(padded);
    return result;
}

unsigned char* gaussian_filter(unsigned char *image, int width, int height)
{
    const int pad   = 1;
    const int pad_w = width  + 2 * pad;
    const int pad_h = height + 2 * pad;

    int *padded = (int*) calloc(pad_w * pad_h, sizeof(int));
    if (!padded) return NULL;

    for (int y = 0; y < height; y++)
        for (int x = 0; x < width; x++) {
            int idx = (y * width + x) * 3;
            padded[(y + pad) * pad_w + (x + pad)] =
                (77 * image[idx] + 150 * image[idx+1] + 29 * image[idx+2]) >> 8;
        }

    unsigned char *result = (unsigned char*) malloc(width * height * 3);
    if (!result) { free(padded); return NULL; }

    for (int y = pad; y < pad_h - pad; y++) {
        for (int x = pad; x < pad_w - pad; x++) {

            int val = 0;
            for (int k = 0; k < 3; k++)
                for (int l = 0; l < 3; l++)
                    val += KERNEL_GAUSS[k][l] * padded[(y - pad + k) * pad_w + (x - pad + l)];

            val /= 16;

            int out_idx = ((y - pad) * width + (x - pad)) * 3;
            result[out_idx + 0] = val;
            result[out_idx + 1] = val;
            result[out_idx + 2] = val;
        }
    }

    free(padded);
    return result;
}