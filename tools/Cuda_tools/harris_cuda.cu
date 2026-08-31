#include "harris_cuda.h"
#include "image_process_cuda.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <cuda_runtime.h>

#define K 0.04

#define IDX(x,y,w) ((y)*(w)+(x))



// Kernel: convertir a float (usar solo canal R)

__global__ void k_to_float(const unsigned char* gray, float* img, int size)
{
    int i = blockIdx.x * blockDim.x + threadIdx.x;
    if (i < size) {
        img[i] = gray[i*3];
    }
}


// Kernel: productos Ixx, Iyy, Ixy

__global__ void k_products(const float* Ix, const float* Iy, float* Ixx, float* Iyy, float* Ixy, int size)
{
    int i = blockIdx.x * blockDim.x + threadIdx.x;
    if (i < size) {
        Ixx[i] = Ix[i]*Ix[i];
        Iyy[i] = Iy[i]*Iy[i];
        Ixy[i] = Ix[i]*Iy[i];
    }
}


// Kernel: Harris response
__global__ void k_harris_response(const float* Sxx, const float* Syy, const float* Sxy, float* R, int size)
{
    int i = blockIdx.x * blockDim.x + threadIdx.x;
    if (i < size) {
        float det = Sxx[i]*Syy[i] - Sxy[i]*Sxy[i];
        float trace = Sxx[i] + Syy[i];
        R[i] = det - K * trace * trace;
    }
}

//Función para probar que no haya errores

#define CUDA_CHECK(call) \
    do { \
        cudaError_t err__ = (call); \
        if (err__ != cudaSuccess) { \
            fprintf(stderr, "CUDA error %s:%d: %s\n", __FILE__, __LINE__, cudaGetErrorString(err__)); \
            exit(1); \
        } \
    } while (0)


/* Non-Maximum Suppression
 (se mantiene en cpu: es secuencial por la salida temprana en max_points
  y por el llenado ordenado de points_x/points_y)
*/ 
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

                    // Se usa >= para que un punto no sea considerado máximo
                    // cuando otro píxel vecino tiene exactamente el mismo valor.
                    if (R[IDX(x+kx,y+ky,w)] >= val) {
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

                // Este mensaje permite comprobar directamente qué esquinas
                // está encontrando el detector Harris.
                printf("Esquina Harris: (%d, %d) R=%f\n", x, y, val);

                p++;

                if (p >= max_points) {
                    printf("Se alcanzo el maximo de %d esquinas.\n", max_points);
                    return;
                }
            }
        }
    }

    // Se muestra cuantas esquinas se encontraron antes de terminar el recorrido.
    printf("Esquinas Harris encontradas: %d\n", p);
}


// Harris 

void harris_detect(unsigned char* gray, int width, int height, float threshold, int max_points, int *points_x, int *points_y)
{
    int size = width * height;

    // Guardamos memoria en cpu para sobel, gauss y non max
    float* h_img  = (float*) malloc(sizeof(float)*size);
    float* h_blur = (float*) malloc(sizeof(float)*size);

    float* h_Ix = (float*) malloc(sizeof(float)*size);
    float* h_Iy = (float*) malloc(sizeof(float)*size);

    float* h_Ixx = (float*) malloc(sizeof(float)*size);
    float* h_Iyy = (float*) malloc(sizeof(float)*size);
    float* h_Ixy = (float*) malloc(sizeof(float)*size);

    float* h_Sxx = (float*) malloc(sizeof(float)*size);
    float* h_Syy = (float*) malloc(sizeof(float)*size);
    float* h_Sxy = (float*) malloc(sizeof(float)*size);

    float* h_R = (float*) malloc(sizeof(float)*size);

    // Lo mismo pero ahora para gpu
    unsigned char* d_gray;
    float *d_img, *d_Ix, *d_Iy, *d_Ixx, *d_Iyy, *d_Ixy, *d_Sxx, *d_Syy, *d_Sxy, *d_R;

    CUDA_CHECK(cudaMalloc(&d_gray, sizeof(unsigned char)*size*3));
    CUDA_CHECK(cudaMalloc(&d_img,  sizeof(float)*size));

    CUDA_CHECK(cudaMalloc(&d_Ix, sizeof(float)*size));
    CUDA_CHECK(cudaMalloc(&d_Iy, sizeof(float)*size));

    CUDA_CHECK(cudaMalloc(&d_Ixx, sizeof(float)*size));
    CUDA_CHECK(cudaMalloc(&d_Iyy, sizeof(float)*size));
    CUDA_CHECK(cudaMalloc(&d_Ixy, sizeof(float)*size));

    CUDA_CHECK(cudaMalloc(&d_Sxx, sizeof(float)*size));
    CUDA_CHECK(cudaMalloc(&d_Syy, sizeof(float)*size));
    CUDA_CHECK(cudaMalloc(&d_Sxy, sizeof(float)*size));

    CUDA_CHECK(cudaMalloc(&d_R, sizeof(float)*size));

    //Parametros del grid
    int blockSize = 256;
    int gridSize = (size + blockSize - 1) / blockSize;

    // 0. subir gray y convertir a float en gpu
    CUDA_CHECK(cudaMemcpy(d_gray, gray, sizeof(unsigned char)*size*3, cudaMemcpyHostToDevice));

    k_to_float<<<gridSize, blockSize>>>(d_gray, d_img, size);
    CUDA_CHECK(cudaGetLastError());
    CUDA_CHECK(cudaDeviceSynchronize());

    CUDA_CHECK(cudaMemcpy(h_img, d_img, sizeof(float)*size, cudaMemcpyDeviceToHost));

    // 1. Gaussian blur (cpu)
    gaussian_filter_cuda(h_blur, h_img, width, height);

    // 2. Gradientes (cpu)
    sobel_filter_cuda(h_Ix, h_Iy, h_blur, width, height);

    // 3. Productos (gpu)
    CUDA_CHECK(cudaMemcpy(d_Ix, h_Ix, sizeof(float)*size, cudaMemcpyHostToDevice));
    CUDA_CHECK(cudaMemcpy(d_Iy, h_Iy, sizeof(float)*size, cudaMemcpyHostToDevice));

    k_products<<<gridSize, blockSize>>>(d_Ix, d_Iy, d_Ixx, d_Iyy, d_Ixy, size);
    CUDA_CHECK(cudaGetLastError());
    CUDA_CHECK(cudaDeviceSynchronize());

    CUDA_CHECK(cudaMemcpy(h_Ixx, d_Ixx, sizeof(float)*size, cudaMemcpyDeviceToHost));
    CUDA_CHECK(cudaMemcpy(h_Iyy, d_Iyy, sizeof(float)*size, cudaMemcpyDeviceToHost));
    CUDA_CHECK(cudaMemcpy(h_Ixy, d_Ixy, sizeof(float)*size, cudaMemcpyDeviceToHost));

    // 4. Blur/filter otra vez (estructura tensor, cpu)
    gaussian_filter_cuda(h_Sxx, h_Ixx, width, height);
    gaussian_filter_cuda(h_Syy, h_Iyy, width, height);
    gaussian_filter_cuda(h_Sxy, h_Ixy, width, height);

    // 5. Harris response (gpu) 
    CUDA_CHECK(cudaMemcpy(d_Sxx, h_Sxx, sizeof(float)*size, cudaMemcpyHostToDevice));
    CUDA_CHECK(cudaMemcpy(d_Syy, h_Syy, sizeof(float)*size, cudaMemcpyHostToDevice));
    CUDA_CHECK(cudaMemcpy(d_Sxy, h_Sxy, sizeof(float)*size, cudaMemcpyHostToDevice));

    k_harris_response<<<gridSize, blockSize>>>(d_Sxx, d_Syy, d_Sxy, d_R, size);
    CUDA_CHECK(cudaGetLastError());
    CUDA_CHECK(cudaDeviceSynchronize());

    CUDA_CHECK(cudaMemcpy(h_R, d_R, sizeof(float)*size, cudaMemcpyDeviceToHost));

    // Se obtiene el rango de la respuesta Harris para comprobar
    // que el threshold utilizado sea adecuado para los valores reales.
    float minR = h_R[0];
    float maxR = h_R[0];

    for (int i = 1; i < size; i++)
    {
        if (h_R[i] < minR)
            minR = h_R[i];

        if (h_R[i] > maxR)
            maxR = h_R[i];
    }

    printf("Harris R: min = %f, max = %f\n", minR, maxR);
    printf("Harris threshold: %f\n", threshold);

    //  6. NMS + dibujar esquinas (cpu)
    non_max_suppression(h_R, width, height, gray, threshold, max_points, points_x, points_y);

    // liberamos memoria (gpu)
    cudaFree(d_gray); cudaFree(d_img);
    cudaFree(d_Ix); cudaFree(d_Iy);
    cudaFree(d_Ixx); cudaFree(d_Iyy); cudaFree(d_Ixy);
    cudaFree(d_Sxx); cudaFree(d_Syy); cudaFree(d_Sxy);
    cudaFree(d_R);

    // liberamos memoria cpu
    free(h_img); free(h_blur);
    free(h_Ix); free(h_Iy);
    free(h_Ixx); free(h_Iyy); free(h_Ixy);
    free(h_Sxx); free(h_Syy); free(h_Sxy);
    free(h_R);
}