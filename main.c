#include <stdlib.h>
#include <stdio.h>
#include "tools/image_descriptors.h"
#include "tools/image_tools.h"
#include "tools/harris.h"


int main()
{
    int width, height, channels;
    unsigned char *img, *gray;

    img = image_load("img/graf/img1.png", &width, &height, &channels);

    if (!img) {
        printf("Error loading image\n");
        return 1;
    }

    printf("Image loaded: width: %d height: %d channels: %d\n", width, height, channels);

    // Convertir a escala de grises (en formato RGB)
    gray = image_to_gray_vector(img, width, height);

    // Aplicar detector de Harris
    //float threshold = 1e7;
    //harris_detect(gray, width, height, threshold);

    draw_rectangle(gray, width, height, 100, 100, 200, 200);
    for (int i = 0; i < 10; i++) {
        int x = sample_point(4, 150, 100);
        int y = sample_point(4, 150, 100);
        draw_point(gray, width, height, x, y);
    }

    // Mostrar resultado (esquinas en rojo)
    image_show(gray, width, height);

    free(img);
    free(gray);

    return 0;
}