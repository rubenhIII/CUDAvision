#include <cuda_runtime.h>
#include <stdio.h>
#include <stdlib.h>

/* Avance

//Plantear kernel y funcion cuda
void conv_cuda(
    float *salida,
    const float *entrada,
    int width,
    int height,
    const float *kernel,
    int half_k,
    float scale)
    


    // Ejecutar GPU
    convolve_float_cuda<<<grid, block>>>(
        d_out,
        d_in,
        width,
        height,
        d_kernel,
        half_k,
        scale
    );

    // GPU a CPU
    cudaMemcpy(
        out,
        d_out,
        size * sizeof(float),
        cudaMemcpyDeviceToHost
    );

    //Liberar memoria
    

}
*/