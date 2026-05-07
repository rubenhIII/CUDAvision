#include <stdlib.h>
#include <stdio.h>
#include "tools/image_descriptors.h"
#include "tools/image_tools.h"
#include "tools/harris.h"


int main()
{
    int width, height, channels;
    unsigned char *img, *gray, *img2, *gray2;

    img = image_load("img/iglesia.jpg", &width, &height, &channels);
    //img2 = image_load("img/graf/img4.png", &width, &height, &channels);

    if (!img) {
        printf("Error loading image\n");
        return 1;
    }

    printf("Image loaded: width: %d height: %d channels: %d\n", width, height, channels);

    // Convertir a escala de grises (en formato RGB)
    gray = image_to_gray_vector(img, width, height);
    //gray2 = image_to_gray_vector(img2, width, height);

    // Aplicar detector de Harris
    float threshold = 1e7;
    int max_points = 10;
    int *points_x = (int *) malloc (sizeof(int) * max_points);
    int *points_y = (int *) malloc (sizeof(int) * max_points);
    harris_detect(gray, width, height, threshold, max_points, points_x, points_y);

    //----
    /*

    for (int i =0; i < max_points; i++) {
        draw_point(gray, width, height, points_x[i], points_y[i], GREEN);
    }

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

    // Stack imágenes
    unsigned char *out_gray = vec_zeros(width * height * 6);
    image_hstack(gray, gray2, out_gray, width, height);
    */

    // Mostrar resultado
    image_show(gray, width, height);

    free(img);
    free(gray);

    return 0;
}