#include "harris.h"
#include "image_process.h"
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#define K 0.04

#define IDX(x,y,w) ((y)*(w)+(x))



// -----------------------------
// Non-Maximum Suppression
// -----------------------------
void non_max_suppression(float* R, int w, int h, unsigned char* out, float threshold, int max_points, int *points_x, int *points_y)
{
    int p = 0;
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

                points_x[p] = x;
                points_y[p] = y;
                p++;
                
                if (p >= max_points) return;
            }
        }
    }
}

// -----------------------------
// Harris completo
// -----------------------------
void harris_detect(unsigned char* gray, int width, int height, float threshold, int max_points, int *points_x, int *points_y)
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
    gaussian_filter(blur, img, width, height);

    // 2. Gradientes
    sobel_filter(Ix, Iy, blur, width, height);

    // 3. Productos
    for (int i = 0; i < size; i++) {
        Ixx[i] = Ix[i]*Ix[i];
        Iyy[i] = Iy[i]*Iy[i];
        Ixy[i] = Ix[i]*Iy[i];
    }

    // 4. Blur/filter otra vez (estructura tensor)
    gaussian_filter(Sxx, Ixx, width, height);
    gaussian_filter(Syy, Iyy, width, height);
    gaussian_filter(Sxy, Ixy, width, height);

    // 5. Harris response
    for (int i = 0; i < size; i++) {
        float det = Sxx[i]*Syy[i] - Sxy[i]*Sxy[i];
        float trace = Sxx[i] + Syy[i];
        R[i] = det - K * trace * trace;
    }

    // 6. NMS + dibujar esquinas
    non_max_suppression(R, width, height, gray, threshold, max_points, points_x, points_y);

    free(img); free(blur);
    free(Ix); free(Iy);
    free(Ixx); free(Iyy); free(Ixy);
    free(Sxx); free(Syy); free(Sxy);
    free(R);
}