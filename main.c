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

    lena = image_load("img/lena.jpeg", &width, &height, &channels);

    printf("Image loaded: width: %d height: %d", width, height);

    lena_gray = image_to_gray_vector(lena, width, height);
    image_show(lena_gray, width, height);

    free(lena);
    free(lena_gray);

    return 0;
}