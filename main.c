/*
    Código de ejemplo de uso de la conversión a escala de grises.
    Carga la imagen de lena.jpeg en la carpeta de imagenes img/.
    Muestra en una ventana el resultado.
*/

#include <stdlib.h>
#include <stdio.h>
#include "tools/image_tools.h"

int main()
{

    int width, height, channels;
    unsigned char *lena, *lena_gray;
    float *hist;

    lena = image_load("img/iglesia.jpg", &width, &height, &channels);

    printf("Image loaded: width: %d height: %d channels: %d\n", width, height, channels);

    lena_gray = image_to_gray_vector(lena, width, height);
    image_show(lena_gray, width, height);
    
    hist = image_hist_norm(lena_gray, width, height);
    unsigned int threshold = otsu_thresholding(hist);
    printf("Best threshold at: %d \n", threshold);

    image_threshold(threshold, lena_gray, width, height);
    image_show(lena_gray, width, height);

    free(lena);
    free(lena_gray);
    free(hist);

    return 0;
}