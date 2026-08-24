#include "image_process_cuda.h"
#include <cuda_runtime.h>
#include <stdio.h>

//Kernel gaussiano

__global__ void gaussian_kernel(
    float *salida,
    const float *entrada,
    int ancho,
    int altura
)
{
    int x = blockIdx.x * blockDim.x + threadIdx.x;
    int y = blockIdx.y * blockDim.y + threadIdx.y;

    // Verificar que el pixel esté dentro de la imagen
    if (x >= ancho || y >= altura)
        return;

    // Kernel Gaussiano 3x3
    const float kernel[3][3] = {
        {1.0f, 2.0f, 1.0f},
        {2.0f, 4.0f, 2.0f},
        {1.0f, 2.0f, 1.0f}
    };

    float suma = 0.0f;

    // Aplicar convolución 3x3
    for (int ky = -1; ky <= 1; ky++)
    {
        for (int kx = -1; kx <= 1; kx++)
        {
            int px = x + kx;
            int py = y + ky;

            // Manejo sencillo de bordes
            if (px >= 0 && px < ancho &&
                py >= 0 && py < altura)
            {
                float valor = entrada[py * ancho + px];

                suma += valor * kernel[ky + 1][kx + 1];
            }
        }
    }

    // Normalización del kernel Gaussiano
    salida[y * ancho + x] = suma / 16.0f;
}

//Funcion gauss cuda

void gaussian_filter_cuda(
    float *salida,
    const float *entrada,
    int ancho,
    int altura
)
{
    float *d_entrada;
    float *d_salida;

    size_t tamano = ancho * altura * sizeof(float); //float ocupa 4 bytes por pixel

    // Reservar memoria en GPU
    cudaMalloc((void**)&d_entrada, tamano);
    cudaMalloc((void**)&d_salida, tamano);

    // Copiar imagen CPU -> GPU
    cudaMemcpy(
        d_entrada,
        entrada,
        tamano,
        cudaMemcpyHostToDevice
    );

    // Definir tamaño de los bloques
    dim3 bloque(16, 16);

    // Calcular cantidad de bloques
    dim3 grid(
        (ancho + bloque.x - 1) / bloque.x,
        (altura + bloque.y - 1) / bloque.y
    );

    // Ejecutar kernel
    gaussian_kernel<<<grid, bloque>>>(
        d_salida,
        d_entrada,
        ancho,
        altura
    );


    cudaDeviceSynchronize();

    // Copiar GPU -> CPU
    cudaMemcpy(
        salida,
        d_salida,
        tamano,
        cudaMemcpyDeviceToHost
    );

    // Liberar memoria GPU
    cudaFree(d_entrada);
    cudaFree(d_salida);
}



// Kernel sobel

__global__ void sobel_kernel(
	//Las dos entradas filtro gx y gy
    float *Ix,
    float *Iy,
    const float *in,
    int width,
    int height
)
{
    int x = blockIdx.x * blockDim.x + threadIdx.x;
    int y = blockIdx.y * blockDim.y + threadIdx.y;

    if (x >= width || y >= height)
        return;

    // Kernels 
    const float Gx[3][3] = {
        {-1.0f, 0.0f, 1.0f},
        {-2.0f, 0.0f, 2.0f},
        {-1.0f, 0.0f, 1.0f}
    };

    const float Gy[3][3] = {
        {-1.0f, -2.0f, -1.0f},
        { 0.0f,  0.0f,  0.0f},
        { 1.0f,  2.0f,  1.0f}
    };

    float sumaX = 0.0f;
    float sumaY = 0.0f;

    // Convolución 3x3
    for (int ky = -1; ky <= 1; ky++)
    {
        for (int kx = -1; kx <= 1; kx++)
        {
            int px = x + kx;
            int py = y + ky;

            if (px >= 0 && px < width &&
                py >= 0 && py < height)
            {
                float pixel = in[py * width + px];

                sumaX += pixel * Gx[ky + 1][kx + 1];
                sumaY += pixel * Gy[ky + 1][kx + 1];
            }
        }
    }

    // Guardar gradientes
    Ix[y * width + x] = sumaX;
    Iy[y * width + x] = sumaY;
}



// Función sobel cuda


void sobel_filter_cuda(
    float *Ix,
    float *Iy,
    const float *in,
    int width,
    int height
)
{
    float *d_Ix;
    float *d_Iy;
    float *d_in;

    size_t tamano = width * height * sizeof(float);

    // Reservar memoria GPU
    cudaMalloc((void**)&d_Ix, tamano);
    cudaMalloc((void**)&d_Iy, tamano);
    cudaMalloc((void**)&d_in, tamano);

    // Copiar imagen CPU -> GPU
    cudaMemcpy(
        d_in,
        in,
        tamano,
        cudaMemcpyHostToDevice
    );

    // Bloques de 16x16 threads
    dim3 bloque(16, 16);

    // Grid suficiente para cubrir toda la imagen
    dim3 grid(
        (width + bloque.x - 1) / bloque.x,
        (height + bloque.y - 1) / bloque.y
    );

    // Ejecutar kernel
    sobel_kernel<<<grid, bloque>>>(
        d_Ix,
        d_Iy,
        d_in,
        width,
        height
    );

    // Esperar GPU
    cudaDeviceSynchronize();

    // Copiar resultados GPU -> CPU
    cudaMemcpy(
        Ix,
        d_Ix,
        tamano,
        cudaMemcpyDeviceToHost
    );

    cudaMemcpy(
        Iy,
        d_Iy,
        tamano,
        cudaMemcpyDeviceToHost
    );

    // Liberar memoria
    cudaFree(d_Ix);
    cudaFree(d_Iy);
    cudaFree(d_in);
}