#include <stdlib.h>
#include <stdio.h>
#include "tools/image_tools.h"
#include "tools/harris.h"

int main()
{
    int width, height, channels;
    unsigned char *img, *gray;

    img = image_load("img/iglesia.jpg", &width, &height, &channels);

    if (!img) {
        printf("Error loading image\n");
        return 1;
    }

    printf("Image loaded: width: %d height: %d channels: %d\n", width, height, channels);

    // Convertir a escala de grises (en formato RGB)
    gray = image_to_gray_vector(img, width, height);

    // Aplicar detector de Harris
    float threshold = 1e8;
    harris_detect(gray, width, height, threshold);

    // Mostrar resultado (esquinas en rojo)
    image_show(gray, width, height);

    free(img);
    free(gray);

    return 0;
}