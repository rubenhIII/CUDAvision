#include<stdio.h>
#include <stdlib.h>

__global__
void ImprimirHilo(void){
    int col = blockIdx.x*blockDim.x +threadIdx.x;
    int row = blockIdx.y*blockDim.y +threadIdx.y;
    printf("Hola desde el hilo en la fila %d y columna %d\n", row, col);
}

int main(){

    dim3 grid(2,1,1);
    dim3 block(2,1,1);

    ImprimirHilo<<<grid, block>>>();
    cudaDeviceSynchronize();

    return 0;
}
//Tarea
/*
Convolucion. correlacion

kernel de tamaño x pasarlo a una matriz de tamaño x

multiplicar y sumar

filtro


*/