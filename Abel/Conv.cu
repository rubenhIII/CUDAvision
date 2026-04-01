#include <stdio.h>
#include <stdlib.h>
#include <math.h>

//Funcion Padding
void CrearPadding(int *matrizOriginal, int *matrizNueva, int filas, int columnas, int padding){
    int nuevasFilas = filas + 2*padding;
    int nuevasColumnas = columnas + 2*padding;
    int constantePadding=0;
    
    //Primero llenamos de ceros
    for(int i = 0; i < nuevasFilas; i++){
        for(int j = 0; j < nuevasColumnas; j++){
            matrizNueva[i*nuevasColumnas+j] = constantePadding;
        }
    }
    //Luego llenamos la matrizpadding con los valores de la matriz original, copiandola en el medio
    for(int i = 0; i < filas; i++){
        for(int j = 0; j < columnas; j++){
            //Recorremos la matriz de 5*5 que esta en medio de la matriz de 7*7
            matrizNueva[(i+padding)*nuevasColumnas+(j+padding)] = matrizOriginal[i*columnas+j];
            //(i+padding) y (j+padding) es para imprimir sin tomar en cuenta las filas de ceros
            //para el padding, es deccir si la matriz es de 7*7 usamos solo la de 5*5
        }
    }
}
//Imprimir matriz int
void imprimirMatriz(int *matriz, int filas , int columnas){
    for(int i = 0; i < filas; i++){
        for(int j = 0; j < columnas; j++){
            printf("%d \t", matriz[i*columnas+j]);
        }
        printf("\n");
    }
}
//Imprimir matriz de grad con double
void imprimirDouble(double *matriz, int filas , int columnas){
    for(int i = 0; i < filas; i++){
        for(int j = 0; j < columnas; j++){
            printf("%f \t", matriz[i*columnas+j]);
        }
        printf("\n");
    }
}



//Funcion para convolucion

void correlacion(int *matrizPad, int *matKernel, int *matrizNueva, int filpad, int colpad, int filkernel, int colkernel, int padding){
    int i = 0, j = 0, valorNuevo;
    //Vamos a recorrer unicamente la matriz sin contar la col y fil extra del padding
    for((i=padding); i<(filpad-(padding)); i++){
        for((j=padding); j < (colpad-(padding)); j++){
            valorNuevo=0;
            /*Usamos posiciones establecidas de la matriz kernel

            0 1 2
            3 4 5
            6 7 8

            el 4 va a ser nuestro centro que posicionaremos en 
            (i+padding)*fila + (j+padding) en la matrizPad puesto que
            no consideramos las filas y columnas extras del padding

            col es el numero de elementos por fila

            Las posiciones completas seran
            (i-padding)*col+j-padding)    (i-padding)*col+(j))      ((i-padding)*col+(j+padding))   
            ((i)*col+j-padding)          ((i)*col+(j))            ((i)*col+(j+padding))
            ((i+padding)*col+j-padding)  ((i+padding)*col+(j))    ((i+padding+1)*col+(j+padding))

            En este caso como empezamos con padding pues nuestros ciclos for empiezan ya 
            con padding, por eso le restamos el padding ya que unicamente tomamos 
            la matriz interna quitando el borde de padding y el i,j ya vienen con padding
            incluido

            */
            for(int k = 0; k < filkernel; k++){
                for(int l = 0; l < colkernel; l++){
                    valorNuevo += matKernel[k*colkernel+l]*matrizPad[(i-padding+k)*colpad+(j-padding+l)];
                }
            }
            /*
            visualizacion del patron
            //Fila medio
            valorNuevo += matKernel[3]*matrizPad[(i)*colpad+(j-padding)];
            valorNuevo += matKernel[4]*matrizPad[(i)*colpad+(j)];
            valorNuevo += matKernel[5]*matrizPad[(i)*colpad+(j+padding)];
            //Fila arriba
            valorNuevo += matKernel[0]*matrizPad[(i-padding)*colpad+(j-padding)];
            valorNuevo += matKernel[1]*matrizPad[(i-padding)*colpad+(j)];
            valorNuevo += matKernel[2]*matrizPad[(i-padding)*colpad+(j+padding)];
            //Fila abajo
            valorNuevo += matKernel[6]*matrizPad[(i+padding)*colpad+(j-padding)];
            valorNuevo += matKernel[7]*matrizPad[(i+padding)*colpad+(j)];
            valorNuevo += matKernel[8]*matrizPad[(i+padding)*colpad+(j+padding)];
            */

            //Colocamos el nuevo valor en la matrizPad
            matrizNueva[(i-padding)*(colpad-2*padding) + (j-padding)] = valorNuevo;
            //reiniciamos el valor
            valorNuevo=0;
        }
    }

}


void Gradiente(int *matrizA, int *matrizB, double *matrizDestino, int fil, int col){
    for(int i=0; i<fil; i++){
        for(int j=0; j<col; j++){
            matrizDestino[i*col+j] = sqrt(matrizA[i*col+j]*matrizA[i*col+j] + matrizB[i*col+j]*matrizB[i*col+j]);
        }
    }
}


int main(){
    //Declaramos una matriz y variables a usar

    const int filas = 5, columnas = 5, padding = 1;
    const int filkernel=3, colkernel=3;
    int mat[filas][columnas]= {{1,1,1,1,1},
                               {1,1,1,1,1},
                               {1,1,1,1,1},
                               {1,1,1,1,1},
                               {1,1,1,1,1}};

    int kernelGX[filkernel][colkernel]={{-1,0,1},
                                        {-2,0,2},
                                        {-1,0,1}};

    int kernelGY[filkernel][colkernel]={{-1,-2,-1},
                                        {0,0,0},
                                        {1,2,1}};

    int filasPadding = filas + 2 * padding;
    int columnasPadding = columnas + 2 * padding;


    //Memoria en cpu

    int *matriz = (int*) malloc(filas*columnas*sizeof(int));
    //reservamos para nueva matriz

    int *matrizPadding = (int*) malloc(filasPadding*columnasPadding*sizeof(int));

    //Para la convolucion generamos una matriz kernel, la que va ir moviendose 
    //Por la matriz con padding y una matrizPosfiltro con un tamaño igual al
    //de la matriz original sin el padding, el padding solo ayuda a la hora
    //de realizar la convolucion
    int *matrizKernelGX = (int*) malloc(filkernel*colkernel*sizeof(int));
    int *matrizKernelGY = (int*) malloc(filkernel*colkernel*sizeof(int));
    int *matrizPosGX = (int*) malloc(filas*columnas*sizeof(int));
    int *matrizPosGY = (int*) malloc(filas*columnas*sizeof(int));
    //Para el gradiente
    double *matrizGradiente = (double*) malloc(filas*columnas*sizeof(double));

    //Guardamos los valores en la memoria dinamica
    for(int i = 0; i < filas; i++){
        for(int j = 0; j < columnas; j++){
            matriz[i*columnas+j] = mat[i][j];
        }
    }
    //Tambien llenamos los kernels
    for(int i = 0; i < filkernel; i++){
        for(int j = 0; j < colkernel; j++){
            matrizKernelGX[i*colkernel+j] = kernelGX[i][j];
        }
    }
    for(int i = 0; i < filkernel; i++){
        for(int j = 0; j < colkernel; j++){
            matrizKernelGY[i*colkernel+j] = kernelGY[i][j];
        }
    }

    //Creamos el padding
    CrearPadding(matriz, matrizPadding, filas, columnas, padding);
    //Matriz sin padding, la original
    imprimirMatriz(matriz,filas,columnas);
    printf("\n");
    //Matriz con padding
    imprimirMatriz(matrizPadding,filasPadding,columnasPadding);
    printf("\n");

    //Generamos la convolucion para gx
    correlacion(matrizPadding, matrizKernelGX, matrizPosGX, filasPadding, columnasPadding, filkernel, colkernel, padding);
    correlacion(matrizPadding, matrizKernelGY, matrizPosGY, filasPadding, columnasPadding, filkernel, colkernel, padding);
    
    printf("Convolucion gx:");
    printf("\n");
    imprimirMatriz(matrizPosGX, filas, columnas);

    printf("Convolucion gy:");
    printf("\n");
    imprimirMatriz(matrizPosGY, filas, columnas);

    printf("Matriz Gradiente: \n");
    Gradiente(matrizPosGX, matrizPosGY, matrizGradiente, filas, columnas);
    imprimirDouble(matrizGradiente, filas, columnas);


    //Liberar memoria
    free(matriz);
    free(matrizPadding);
    free(matrizKernelGX);
    free(matrizKernelGY);
    free(matrizPosGX);
    free(matrizPosGY);
    free(matrizGradiente);

    return 0;
}

