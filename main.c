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

    draw_rectangle(gray, width, height, 400, 400, 500, 500);
    for (int i = 0; i < 255; i++) {
        int x0 = sample_point(20, 450, 100);
        int y0 = sample_point(20, 450, 100);

        int x1 = sample_point(20, 450, 100);
        int y1 = sample_point(20, 450, 100);
        
        int res = binary_test(gray, width, height, x0, y0, x1, y1);

        draw_point(gray, width, height, x0, y0, RED);
        draw_point(gray, width, height, x1, y1, BLUE);

        printf("%d ", res);
    }
    printf("\n");

    // Mostrar resultado (esquinas en rojo)
    image_show(gray, width, height);

    free(img);
    free(gray);

    return 0;
}