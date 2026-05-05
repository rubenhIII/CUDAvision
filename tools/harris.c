#include "harris.h"
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#define K 0.04

#define IDX(x,y,w) ((y)*(w)+(x))

// -----------------------------
// Gaussian blur (kernel 3x3)
// -----------------------------
void gaussian_blur(float* out, float* in, int w, int h)
{
    float kernel[3][3] = {
        {1, 2, 1},
        {2, 4, 2},
        {1, 2, 1}
    };

    for (int y = 1; y < h-1; y++) {
        for (int x = 1; x < w-1; x++) {

            float sum = 0.0;
            float norm = 0.0;

            for (int ky = -1; ky <= 1; ky++) {
                for (int kx = -1; kx <= 1; kx++) {

                    float k = kernel[ky+1][kx+1];
                    sum += in[IDX(x+kx,y+ky,w)] * k;
                    norm += k;
                }
            }

            out[IDX(x,y,w)] = sum / norm;
        }
    }
}

// -----------------------------
// Sobel
// -----------------------------
void sobel(float* Ix, float* Iy, float* img, int w, int h)
{
    int Gx[3][3] = {
        {-1,0,1},{-2,0,2},{-1,0,1}
    };

    int Gy[3][3] = {
        {-1,-2,-1},{0,0,0},{1,2,1}
    };

    for (int y = 1; y < h-1; y++) {
        for (int x = 1; x < w-1; x++) {

            float sx = 0, sy = 0;

            for (int ky=-1; ky<=1; ky++) {
                for (int kx=-1; kx<=1; kx++) {

                    float pixel = img[IDX(x+kx,y+ky,w)];

                    sx += pixel * Gx[ky+1][kx+1];
                    sy += pixel * Gy[ky+1][kx+1];
                }
            }

            Ix[IDX(x,y,w)] = sx;
            Iy[IDX(x,y,w)] = sy;
        }
    }
}

// -----------------------------
// Non-Maximum Suppression
// -----------------------------
void non_max_suppression(float* R, int w, int h, unsigned char* out, float threshold)
{
    for (int y = 1; y < h-1; y++) {
        for (int x = 1; x < w-1; x++) {

            float val = R[IDX(x,y,w)];

            if (val < threshold) continue;

            int is_max = 1;

            for (int ky=-1; ky<=1; ky++) {
                for (int kx=-1; kx<=1; kx++) {

                    if (kx==0 && ky==0) continue;

                    if (R[IDX(x+kx,y+ky,w)] > val) {
                        is_max = 0;
                        break;
                    }
                }
                if (!is_max) break;
            }

            if (is_max) {
                int idx = (y*w + x)*3;
                out[idx+0] = 255; // rojo
                out[idx+1] = 0;
                out[idx+2] = 0;
            }
        }
    }
}

// -----------------------------
// Harris completo
// -----------------------------
void harris_detect(unsigned char* gray, int width, int height, float threshold)
{
    int size = width * height;

    float* img = (float*) malloc(sizeof(float)*size);
    float* blur = (float*) malloc(sizeof(float)*size);

    float* Ix = (float*) malloc(sizeof(float)*size);
    float* Iy = (float*) malloc(sizeof(float)*size);

    float* Ixx = (float*) malloc(sizeof(float)*size);
    float* Iyy = (float*) malloc(sizeof(float)*size);
    float* Ixy = (float*) malloc(sizeof(float)*size);

    float* Sxx = (float*) malloc(sizeof(float)*size);
    float* Syy = (float*) malloc(sizeof(float)*size);
    float* Sxy = (float*) malloc(sizeof(float)*size);

    float* R = (float*) malloc(sizeof(float)*size);

    // convertir a float (usar solo canal R)
    for (int i = 0; i < size; i++) {
        img[i] = gray[i*3];
    }

    // 1. Gaussian blur
    gaussian_blur(blur, img, width, height);

    // 2. Gradientes
    sobel(Ix, Iy, blur, width, height);

    // 3. Productos
    for (int i = 0; i < size; i++) {
        Ixx[i] = Ix[i]*Ix[i];
        Iyy[i] = Iy[i]*Iy[i];
        Ixy[i] = Ix[i]*Iy[i];
    }

    // 4. Blur otra vez (estructura tensor)
    gaussian_blur(Sxx, Ixx, width, height);
    gaussian_blur(Syy, Iyy, width, height);
    gaussian_blur(Sxy, Ixy, width, height);

    // 5. Harris response
    for (int i = 0; i < size; i++) {
        float det = Sxx[i]*Syy[i] - Sxy[i]*Sxy[i];
        float trace = Sxx[i] + Syy[i];
        R[i] = det - K * trace * trace;
    }

    // 6. NMS + dibujar esquinas
    non_max_suppression(R, width, height, gray, threshold);

    free(img); free(blur);
    free(Ix); free(Iy);
    free(Ixx); free(Iyy); free(Ixy);
    free(Sxx); free(Syy); free(Sxy);
    free(R);
}