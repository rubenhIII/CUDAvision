#include <cuda_runtime.h>
#include <stdio.h>
#include <stdlib.h>

__global__ 
void holagpu(void) {
int thread_id = blockIdx.x * blockDim.x + threadIdx.x;
    printf("Hola desde GPU %d\n",thread_id);
}

int main() {

    // Ejecutar kernel
    holagpu<<<2, 2>>>();
    
    cudaDeviceSynchronize();
    
    return 0;
}