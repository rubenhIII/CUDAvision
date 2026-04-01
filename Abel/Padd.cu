#include <stdio.h>
#include <stdlib.h>

#define FILAS 5
#define COLUMNAS 5
#define PADDING 1

void imprimirMatriz(int *matriz, int filas, int columnas) {
    for (int i = 0; i < filas; i++) {
        for (int j = 0; j < columnas; j++) {
            printf("%4d", matriz[i * columnas + j]);
        }
        printf("\n");
    }
}

void crearPadding(int *entrada, int *salidaPadded, int filas, int columnas, int padding) {
    int nuevasFilas = filas + 2 * padding;
    int nuevasColumnas = columnas + 2 * padding;

    for (int i = 0; i < nuevasFilas; i++)
        for (int j = 0; j < nuevasColumnas; j++)
            salidaPadded[i * nuevasColumnas + j] = 0;

    for (int i = 0; i < filas; i++)
        for (int j = 0; j < columnas; j++)
            salidaPadded[(i + padding) * nuevasColumnas + (j + padding)] = entrada[i * columnas + j];
}

int main() {
    int mat[FILAS][COLUMNAS] = {
        {1, 2, 3, 4, 2},
        {2, 3, 5, 6, 7},
        {3, 5, 6, 1, 2},
        {2, 3, 1, 5, 6},
        {2, 4, 5, 6, 7}
    };

    int paddedFilas    = FILAS    + 2 * PADDING; //Ahora es 7*7
    int paddedColumnas = COLUMNAS + 2 * PADDING;

    int *h_matriz      = (int *)malloc(FILAS * COLUMNAS * sizeof(int));
    int *h_matrizPadded = (int *)malloc(paddedFilas * paddedColumnas * sizeof(int));

    for (int i = 0; i < FILAS; i++)
        for (int j = 0; j < COLUMNAS; j++)
            h_matriz[i * COLUMNAS + j] = mat[i][j];

    crearPadding(h_matriz, h_matrizPadded, FILAS, COLUMNAS, PADDING);

    printf("MATRIZ ORIGINAL (5x5):\n");
    imprimirMatriz(h_matriz, FILAS, COLUMNAS);

    printf("\nMATRIZ CON PADDING (7x7):\n");
    imprimirMatriz(h_matrizPadded, paddedFilas, paddedColumnas);

    free(h_matriz);
    free(h_matrizPadded);

    return 0;
}