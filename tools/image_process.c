#include "image_process.h"
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

static const float KERNEL_GX[9] = {
    -1,  0,  1,
    -2,  0,  2,
    -1,  0,  1
};
static const float KERNEL_GY[9] = {
    -1, -2, -1,
     0,  0,  0,
     1,  2,  1
};
static const float KERNEL_GAUSS[9] = {
    1, 2, 1,
    2, 4, 2,
    1, 2, 1
};

unsigned char* image_padding(unsigned char *image, int width, int height, int pad)
{
    int new_w = width  + 2 * pad;
    int new_h = height + 2 * pad;
 
    unsigned char *padded = (unsigned char*) calloc(new_w * new_h * 3, sizeof(unsigned char));
    if (!padded) return NULL;
 
    for (int y = 0; y < height; y++)
        for (int x = 0; x < width; x++)
            for (int c = 0; c < 3; c++)
                padded[((y + pad) * new_w + (x + pad)) * 3 + c] =
                    image[(y * width + x) * 3 + c];
 
    return padded;
}

//CONVOLUCIÓN Float
void convolve_float(float *out, const float *in,
                    int width, int height,
                    const float *kernel, int half_k, float scale)
{
    int ksize = 2 * half_k + 1;
 
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
 
            float acc = 0.0f;
 
            for (int ky = -half_k; ky <= half_k; ky++) {
                for (int kx = -half_k; kx <= half_k; kx++) {
                    int sy = y + ky;
                    int sx = x + kx;
 
                    if (sy < 0 || sy >= height || sx < 0 || sx >= width)
                        continue;   /* padding cero en el borde */
 
                    float kval = kernel[(ky + half_k) * ksize + (kx + half_k)];
                    acc += kval * in[sy * width + sx];
                }
            }
 
            out[y * width + x] = (scale != 0.0f) ? (acc / scale) : acc;
        }
    }
}
 
 
//FILTRO GAUSSIANO  Usando float
void gaussian_filter(float *out, const float *in, int width, int height)
{
    convolve_float(out, in, width, height, KERNEL_GAUSS, 1, 16.0f);
}
 
 
 //FILTRO SOBEL  Usando float 
void sobel_filter(float *Ix, float *Iy, const float *in, int width, int height)
{
    convolve_float(Ix, in, width, height, KERNEL_GX, 1, 0.0f);
    convolve_float(Iy, in, width, height, KERNEL_GY, 1, 0.0f);
}