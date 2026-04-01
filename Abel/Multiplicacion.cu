#include <stdio.h>
#include <stdlib.h>

__global__

void MultiplicacionPorNumero(int *vec, int a, int *vec_destino, int tam){
    int id = blockIdx.x * blockDim.x + threadIdx.x;
    if(id < tam)
        *(vec_destino + id) = *(vec + id)*a;
}

//Funcion para imprimir
void imprimir(int *vec, int tam){
    for(int i = 0; i < tam; i++){
        printf("%d ", *(vec+i));
    }
}


int main(){
    //Podemos hacer que el tamaño sea introducido por el usuario
    //De forma provisional lo dejamos estatico
    int tam = 10;
    int vec_aux [10] = {1,2,3,4,5,6,7,8,9,10};
    int Num = 4;

    //Generamos la memoria dinamica
    int *vec = (int*) malloc(tam*sizeof(int));
    int *vec_resultado = (int*) malloc(tam*sizeof(int));

    //Ahora apuntamos la memoria a los valores que tenemos 
    for(int i = 0; i < tam; i++)
        *(vec + i) = *(vec_aux + i);


    //Alocamos memoria en gpu
    int *vec_gpu;
    int *vec_resultado_gpu;

    cudaMalloc((void **)&vec_gpu, sizeof(int)*tam);
    cudaMalloc((void **)&vec_resultado_gpu, sizeof(int)*tam);

    //Volcamos los datos a la gpu
    cudaMemcpy(vec_gpu, vec, sizeof(int)*tam, cudaMemcpyHostToDevice);
    //Ejecutamos el kernel
    MultiplicacionPorNumero<<<3,4>>>(vec_gpu, Num, vec_resultado_gpu, tam);

    //Esperamos a que termine la gpu
    cudaDeviceSynchronize();

    //Copiamos el resultado a la memoria principal
    cudaMemcpy(vec_resultado, vec_resultado_gpu, sizeof(int)*tam, cudaMemcpyDeviceToHost);




//Liberamos memoria
    free(vec);
    free(vec_resultado);
    cudaFree(vec_gpu);
    cudaFree(vec_resultado_gpu);

    printf("El vector\n");
    imprimir(vec_aux, tam);
    printf("\nFue multiplicado por %d \n", Num);
    printf("El nuevo vector es:\n");
    imprimir(vec_resultado, tam);


    return 0;
}