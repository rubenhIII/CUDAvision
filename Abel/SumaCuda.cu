#include <stdio.h>
#include <stdlib.h>

__global__ 
void Kernel(int *vec1, int *vec2, int *vecResultado, int tam) {
    int thread_id = blockIdx.x * blockDim.x + threadIdx.x;
    if( thread_id < tam){
        vecResultado[thread_id] = vec1[thread_id] + vec2[thread_id];
        printf("El hilo %d sumo %d + %d = %d\n", thread_id, vec1[thread_id], vec2[thread_id], vecResultado[thread_id]);
    }
}

__host__

void Imprimir (int *vec, int size){
    for(int i = 0; i < size; i++){
        printf("%d ", vec[i]);
    }

}
int main(){
    int size=10;
int vec2_aux[10]= {1,2,3,4,5,6,7,8,9,10};
int vec1_aux[10]= {1,1,1,1,1,1,1,1,1,1};

int *vec1 = (int*) malloc (10*sizeof(int));
int *vec2 = (int*) malloc (10*sizeof(int));
int *vec3 = (int*) malloc (10*sizeof(int));

for(int i = 0; i < 10; i++)
    vec1[i] = vec1_aux[i];
for(int i = 0; i < 10; i++)
    vec2[i] = vec2_aux[i]; 

//Alocar memoria en GPU

int *vec1_gpu, *vec2_gpu, *vecSum_gpu;


cudaMalloc((void **)&vec1_gpu, sizeof(int)*size);
cudaMalloc((void **)&vec2_gpu, sizeof(int)*size);
cudaMalloc((void **)&vecSum_gpu, sizeof(int)*size);

//Volcar datos en gpu

cudaMemcpy(vec1_gpu, vec1, sizeof(int)*size, cudaMemcpyHostToDevice);
cudaMemcpy(vec2_gpu, vec2, sizeof(int)*size, cudaMemcpyHostToDevice);
Kernel<<<5,5>>>(vec1_gpu, vec2_gpu, vecSum_gpu, size);
//Esperamos
cudaDeviceSynchronize();
//Una vez que termina gpu, copiamos el resultado a la memoria principal
cudaMemcpy(vec3, vecSum_gpu, sizeof(int)*size, cudaMemcpyDeviceToHost);

//iberamos memoria
free(vec1);
free(vec2);
free(vec3);

cudaFree(vec1_gpu);
cudaFree(vec1_gpu);
cudaFree(vec1_gpu);

//Impresion de resultados

printf("Vectores\n");
Imprimir(vec1, size);
Imprimir(vec2, size);
printf("Resultado de suma \n");
Imprimir(vec3, size);
//Tarea
/*
vector por constante

Make

codigo añadir a una matriz pading
*/

    return 0;
}



