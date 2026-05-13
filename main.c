#include <stdlib.h>
#include <stdio.h>
#include "tools/image_descriptors.h"
#include "tools/image_tools.h"
#include "tools/harris.h"


int main()
{
    int width, height, channels;
    int width2, height2, channels2;
    unsigned char *img, *gray, *img2, *gray2;

    img = image_load("img/lena.jpeg", &width, &height, &channels);
    img2 = image_load("img/lena_translated.jpeg", &width2, &height2, &channels2);

    if (!img) {
        printf("Error loading image\n");
        return 1;
    }

    printf("Image loaded: width: %d height: %d channels: %d\n", width, height, channels);

    // Convertir a escala de grises (en formato RGB)
    gray = image_to_gray_vector(img, width, height);
    gray2 = image_to_gray_vector(img2, width2, height2);

    // Aplicar detector de Harris
    float threshold = 1e7;
    int max_points = 1000;
    int *points_x = (int *) malloc (sizeof(int) * max_points);
    int *points_y = (int *) malloc (sizeof(int) * max_points);
    harris_detect(gray, width, height, threshold, max_points, points_x, points_y);

    int *points_x2 = (int *) malloc (sizeof(int) * max_points);
    int *points_y2 = (int *) malloc (sizeof(int) * max_points);
    harris_detect(gray2, width2, height2, threshold, max_points, points_x2, points_y2);

    //for (int i = 0; i < max_points; i++) {
    //    printf("x: %d y: %d \n", points_x[i], points_y[i]);
    //}


    //----
    // Descriptor BRIEF
    int descriptor_len = 256;
    double **norm_x = matrix_zeros_d(descriptor_len, 2);
    double **norm_y = matrix_zeros_d(descriptor_len, 2);
    unsigned char **descriptors_img1 = matrix_zeros(max_points, descriptor_len);
    unsigned char **descriptors_img2 = matrix_zeros(max_points, descriptor_len);

    double sigma = 20;
    sample_pattern(sigma, norm_x, norm_y, descriptor_len);
    brief_descriptor(gray, width, height, descriptors_img1, descriptor_len, points_x, points_y, max_points, norm_x, norm_y);
    brief_descriptor(gray2, width2, height2, descriptors_img2, descriptor_len, points_x2, points_y2, max_points, norm_x, norm_y);

    // Imprime vectores de descripcion
    /*
    for (int p = 0; p < max_points; p++) {
        for (int d = 0; d < descriptor_len; d++) {
            printf("%d", descriptors_img1[p][d]);
        }
        printf(" ");
        for (int d = 0; d < descriptor_len; d++) {
            printf("%d", descriptors_img2[p][d]);
        }
        printf("\n");
    }
    printf("\n");
    */

    unsigned int **matches = hamming(descriptors_img1, descriptors_img2, descriptor_len, max_points);
    //printf("Matches\n");
    //for (int p = 0; p < max_points; p++) {
    //    printf("%d %d %d\n", matches[p][0], matches[p][1], matches[p][2]);
    //}

    // Libera memoria
    for (int p = 0; p < max_points; p++) {
        free(matches[p]);
    }
    free(matches);
    matrix_free_d(norm_x, descriptor_len, 2);
    matrix_free_d(norm_y, descriptor_len, 2);
    
    
    // Stack imágenes y muestra
    unsigned char *out_gray = vec_zeros(width * height * 6);
    image_hstack(gray, gray2, out_gray, width, height);

    int x_offset = width;
    int good_counter = 0;
    for (int p = 0; p < max_points; p++) {
        if (matches[p][2] <= 1 && good_counter <= 20) {
            printf("Entro\n");
            printf("Matches: %d %d %d %d\n", matches[p][0], matches[p][0], matches[p][1], matches[p][1]);
            good_counter++;
            printf("Points: %d %d %d %d\n\n", points_x[matches[p][0]], points_y[matches[p][0]], points_x2[matches[p][1]], points_y2[matches[p][1]]);
            draw_line(out_gray, width * 2, height, points_x[matches[p][0]], points_y[matches[p][0]], points_x2[matches[p][1]] + x_offset, points_y2[matches[p][1]]);
        }
    }
    image_show(out_gray, width * 2, height);
    
    // Mostrar resultado
    //image_show(gray, width, height);

    free(img);
    free(gray);

    free(points_x);
    free(points_y);
    image_free(descriptors_img1, descriptor_len, max_points);
    image_free(descriptors_img2, descriptor_len, max_points);

    return 0;
}