#include "image_descriptor_cuda.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <cuda_runtime.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

#define CUDA_CHECK(call) \
    do { \
        cudaError_t err__ = (call); \
        if (err__ != cudaSuccess) { \
            fprintf(stderr, "CUDA error %s:%d: %s\n", __FILE__, __LINE__, cudaGetErrorString(err__)); \
            exit(1); \
        } \
    } while (0)

// Números aleatorios (se quedan en host: rand() es secuencial
// y esto solo se ejecuta una vez para generar el patrón BRIEF)

// Genera un número uniforme en (0,1)
double uniform()
{
    return (rand() + 1.0) / (RAND_MAX + 2.0);
}

// Genera un número gaussiano N(0,1)
double normal()
{
    double u1 = uniform();
    double u2 = uniform();

    double z0 = sqrt(-2.0 * log(u1)) * cos(2 * M_PI * u2);
    double z1 = sqrt(-2.0 * log(u1)) * sin(2 * M_PI * u2);

    return z0;
}

double rnorm(double mu, double sigma)
{
    return mu + sigma * normal();
}

int sample_pattern(double sigma, double **norm_x, double **norm_y, int descriptor_len)
{
    if (!norm_x || !norm_y || descriptor_len <= 0)
        return -1;

    for (int d = 0; d < descriptor_len; d++) {
        norm_x[d][0] = sigma * normal();
        norm_x[d][1] = sigma * normal();
        norm_y[d][0] = sigma * normal();
        norm_y[d][1] = sigma * normal();
    }
    return 0;
}

/*
    Utilidades: __host__ __device__ para poder usarlas tanto
    desde código host como dentro de los kernels CUDA
*/

//Genera coordenada con sigma
__host__ __device__ int sample_point(double val, int center_coord, int patch_size, int limit)
{
    int half_patch = (int) patch_size / 2;
    int coord = (int)round(val);

    // Clamp al parche
    if (coord > half_patch) coord = half_patch;
    if (coord < -half_patch) coord = -half_patch;

    coord = center_coord + coord;
    coord = coord < 0 ? 0 : coord % limit;

    return coord;
}

// Prueba binaria entre coordenadas dentro de un parche
__host__ __device__ int binary_test(unsigned char *gray_image, int width, int height, int x0, int y0, int x1, int y1)
{
    int idx0 = (y0 * width + x0) * 3;
    int idx1 = (y1 * width + x1) * 3;

    int result = gray_image[idx0] < gray_image[idx1] ? 1 : 0;
    return result;
}

// Kernel: BRIEF descriptor
// Un hilo por cada (punto p, bit d) del descriptor
__global__ void k_brief_descriptor(
    const unsigned char* gray, int width, int height,
    unsigned char* descriptors_flat, int descriptor_len,
    const int* points_x, const int* points_y, int max_points,
    const double* norm_x_flat, const double* norm_y_flat)
{
    int idx = blockIdx.x * blockDim.x + threadIdx.x;
    int total = max_points * descriptor_len;
    if (idx >= total) return;

    int p = idx / descriptor_len;
    int d = idx % descriptor_len;

    int x = points_x[p];
    int y = points_y[p];

    // norm_x[d][0..1], norm_y[d][0..1] aplanados como norm_x_flat[d*2 + 0/1]
    int x0 = sample_point(norm_x_flat[d*2 + 0], x, 100, width);
    int y0 = sample_point(norm_y_flat[d*2 + 0], y, 100, height);

    int x1 = sample_point(norm_x_flat[d*2 + 1], x, 100, width);
    int y1 = sample_point(norm_y_flat[d*2 + 1], y, 100, height);

    descriptors_flat[p * descriptor_len + d] =
        (unsigned char) binary_test((unsigned char*)gray, width, height, x0, y0, x1, y1);
}

void brief_descriptor(unsigned char * gray, int width, int height, unsigned char **descriptors, int descriptor_len, int *points_x, int *points_y, int max_points, double **norm_x, double **norm_y)
{
    if (!gray ||
        !descriptors ||
        !points_x ||
        !points_y ||
        !norm_x ||
        !norm_y ||
        width <= 0 ||
        height <= 0 ||
        descriptor_len <= 0 ||
        max_points <= 0)
    {
        fprintf(stderr, "Error: parametros invalidos en brief_descriptor.\n");
        return;
    }

    size_t gray_size = (size_t)width * height * 3;
    size_t desc_size = (size_t)max_points * descriptor_len;

    // ---- aplanar norm_x / norm_y (host) ----
    double* h_norm_x = (double*) malloc(sizeof(double) * descriptor_len * 2);
    double* h_norm_y = (double*) malloc(sizeof(double) * descriptor_len * 2);

    if (!h_norm_x || !h_norm_y)
    {
        fprintf(stderr, "Error reservando memoria para patron BRIEF aplanado.\n");

        if (h_norm_x)
            free(h_norm_x);

        if (h_norm_y)
            free(h_norm_y);

        return;
    }

    for (int d = 0; d < descriptor_len; d++) {
        h_norm_x[d*2+0] = norm_x[d][0];
        h_norm_x[d*2+1] = norm_x[d][1];
        h_norm_y[d*2+0] = norm_y[d][0];
        h_norm_y[d*2+1] = norm_y[d][1];
    }

    // ---- aplanar buffer de salida (host) ----
    unsigned char* h_desc_flat = (unsigned char*) malloc(desc_size);

    if (!h_desc_flat)
    {
        fprintf(stderr, "Error reservando memoria para descriptores aplanados.\n");

        free(h_norm_x);
        free(h_norm_y);

        return;
    }

    // ---- buffers device ----
    unsigned char* d_gray;
    unsigned char* d_desc_flat;
    int *d_points_x, *d_points_y;
    double *d_norm_x, *d_norm_y;

    CUDA_CHECK(cudaMalloc(&d_gray, gray_size));
    CUDA_CHECK(cudaMalloc(&d_desc_flat, desc_size));
    CUDA_CHECK(cudaMalloc(&d_points_x, sizeof(int) * max_points));
    CUDA_CHECK(cudaMalloc(&d_points_y, sizeof(int) * max_points));
    CUDA_CHECK(cudaMalloc(&d_norm_x, sizeof(double) * descriptor_len * 2));
    CUDA_CHECK(cudaMalloc(&d_norm_y, sizeof(double) * descriptor_len * 2));

    CUDA_CHECK(cudaMemcpy(d_gray, gray, gray_size, cudaMemcpyHostToDevice));
    CUDA_CHECK(cudaMemcpy(d_points_x, points_x, sizeof(int) * max_points, cudaMemcpyHostToDevice));
    CUDA_CHECK(cudaMemcpy(d_points_y, points_y, sizeof(int) * max_points, cudaMemcpyHostToDevice));
    CUDA_CHECK(cudaMemcpy(d_norm_x, h_norm_x, sizeof(double) * descriptor_len * 2, cudaMemcpyHostToDevice));
    CUDA_CHECK(cudaMemcpy(d_norm_y, h_norm_y, sizeof(double) * descriptor_len * 2, cudaMemcpyHostToDevice));

    int total = max_points * descriptor_len;
    int blockSize = 256;
    int gridSize = (total + blockSize - 1) / blockSize;

    k_brief_descriptor<<<gridSize, blockSize>>>(
        d_gray, width, height,
        d_desc_flat, descriptor_len,
        d_points_x, d_points_y, max_points,
        d_norm_x, d_norm_y);

    CUDA_CHECK(cudaGetLastError());
    CUDA_CHECK(cudaDeviceSynchronize());

    CUDA_CHECK(cudaMemcpy(h_desc_flat, d_desc_flat, desc_size, cudaMemcpyDeviceToHost));

    // ---- desaplanar hacia descriptors[p][d] (que el caller ya reservó) ----
    for (int p = 0; p < max_points; p++) {
        if (descriptors[p])
        {
            memcpy(
                descriptors[p],
                h_desc_flat + (size_t)p * descriptor_len,
                descriptor_len
            );
        }
    }

    cudaFree(d_gray); cudaFree(d_desc_flat);
    cudaFree(d_points_x); cudaFree(d_points_y);
    cudaFree(d_norm_x); cudaFree(d_norm_y);

    free(h_norm_x); free(h_norm_y);
    free(h_desc_flat);
}

// Kernel: Hamming matching
// Un hilo por cada punto i de la primera imagen; recorre todos
// los j de la segunda (misma estructura que el for anidado original)

__global__ void k_hamming(
    const unsigned char* descriptors_flat,
    const unsigned char* descriptors2_flat,
    int descriptor_len, int max_points,
    unsigned int* matches_flat)
{
    int i = blockIdx.x * blockDim.x + threadIdx.x;
    if (i >= max_points) return;

    int best_j = 0;
    int best_distance = 0;

    for (int j = 0; j < max_points; j++) {
        int distance = 0;
        for (int k = 0; k < descriptor_len; k++) {
            int diff = descriptors_flat[i*descriptor_len + k] == descriptors2_flat[j*descriptor_len + k] ? 0 : 1;
            distance += diff;
        }
        if (j == 0 || distance < best_distance) {
            best_j = j;
            best_distance = distance;
        }
    }

    matches_flat[i*3 + 0] = (unsigned int) i;
    matches_flat[i*3 + 1] = (unsigned int) best_j;
    matches_flat[i*3 + 2] = (unsigned int) best_distance;
}

unsigned int** hamming(unsigned char **descriptors, unsigned char **descriptors2, int descriptor_len, int max_points)
{
    if (!descriptors ||
        !descriptors2 ||
        descriptor_len <= 0 ||
        max_points <= 0)
    {
        fprintf(stderr, "Error: parametros invalidos en hamming.\n");
        return NULL;
    }

    size_t desc_size = (size_t)max_points * descriptor_len;

    // ---- aplanar descriptors / descriptors2 (host) ----
    unsigned char* h_desc1_flat = (unsigned char*) malloc(desc_size);
    unsigned char* h_desc2_flat = (unsigned char*) malloc(desc_size);

    if (!h_desc1_flat || !h_desc2_flat)
    {
        fprintf(stderr, "Error reservando memoria para descriptores aplanados.\n");

        if (h_desc1_flat)
            free(h_desc1_flat);

        if (h_desc2_flat)
            free(h_desc2_flat);

        return NULL;
    }

    for (int p = 0; p < max_points; p++) {
        if (!descriptors[p] || !descriptors2[p])
        {
            fprintf(stderr, "Error: descriptor invalido en la posicion %d.\n", p);

            free(h_desc1_flat);
            free(h_desc2_flat);

            return NULL;
        }

        memcpy(
            h_desc1_flat + (size_t)p * descriptor_len,
            descriptors[p],
            descriptor_len
        );

        memcpy(
            h_desc2_flat + (size_t)p * descriptor_len,
            descriptors2[p],
            descriptor_len
        );
    }

    unsigned int* h_matches_flat =
        (unsigned int*) malloc(
            sizeof(unsigned int) * max_points * 3
        );

    if (!h_matches_flat)
    {
        fprintf(stderr, "Error reservando memoria para matches.\n");

        free(h_desc1_flat);
        free(h_desc2_flat);

        return NULL;
    }

    // ---- buffers device ----
    unsigned char *d_desc1, *d_desc2;
    unsigned int* d_matches;

    CUDA_CHECK(cudaMalloc(&d_desc1, desc_size));
    CUDA_CHECK(cudaMalloc(&d_desc2, desc_size));
    CUDA_CHECK(cudaMalloc(&d_matches, sizeof(unsigned int) * max_points * 3));

    CUDA_CHECK(cudaMemcpy(
        d_desc1,
        h_desc1_flat,
        desc_size,
        cudaMemcpyHostToDevice
    ));

    CUDA_CHECK(cudaMemcpy(
        d_desc2,
        h_desc2_flat,
        desc_size,
        cudaMemcpyHostToDevice
    ));

    int blockSize = 256;
    int gridSize = (max_points + blockSize - 1) / blockSize;

    k_hamming<<<gridSize, blockSize>>>(
        d_desc1,
        d_desc2,
        descriptor_len,
        max_points,
        d_matches
    );

    CUDA_CHECK(cudaGetLastError());
    CUDA_CHECK(cudaDeviceSynchronize());

    CUDA_CHECK(cudaMemcpy(
        h_matches_flat,
        d_matches,
        sizeof(unsigned int) * max_points * 3,
        cudaMemcpyDeviceToHost
    ));

    // ---- reconstruir matches[i][0..2] (mismo formato que la versión original) ----
    unsigned int **matches =
        (unsigned int**) malloc(
            sizeof(unsigned int*) * max_points
        );

    if (!matches)
    {
        fprintf(stderr, "Error reservando memoria para matriz de matches.\n");

        cudaFree(d_desc1);
        cudaFree(d_desc2);
        cudaFree(d_matches);

        free(h_desc1_flat);
        free(h_desc2_flat);
        free(h_matches_flat);

        return NULL;
    }

    for (int i = 0; i < max_points; i++) {
        matches[i] =
            (unsigned int *) malloc(
                sizeof(unsigned int) * 3
            );

        if (!matches[i])
        {
            fprintf(
                stderr,
                "Error reservando memoria para match %d.\n",
                i
            );

            for (int j = 0; j < i; j++)
                free(matches[j]);

            free(matches);

            cudaFree(d_desc1);
            cudaFree(d_desc2);
            cudaFree(d_matches);

            free(h_desc1_flat);
            free(h_desc2_flat);
            free(h_matches_flat);

            return NULL;
        }

        matches[i][0] = h_matches_flat[i*3 + 0];
        matches[i][1] = h_matches_flat[i*3 + 1];
        matches[i][2] = h_matches_flat[i*3 + 2];
    }

    cudaFree(d_desc1);
    cudaFree(d_desc2);
    cudaFree(d_matches);

    free(h_desc1_flat);
    free(h_desc2_flat);
    free(h_matches_flat);

    return matches;
}