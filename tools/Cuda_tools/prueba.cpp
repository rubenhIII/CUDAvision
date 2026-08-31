#include <stdio.h>
#include <stdlib.h>
#define STB_IMAGE_IMPLEMENTATION
#include "../stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "../stb_image_write.h"
#include "image_process_cuda.h"
#include "harris_cuda.h"

int main()
{
    // Parámetros de la imagen
    int ancho;
    int altura;
    int canales;

    // 1. Cargar imagen en escala de grises (1 canal)
    unsigned char *imagen = stbi_load(
        "imagen.jpg",
        &ancho,
        &altura,
        &canales,
        1
    );

    if (imagen == NULL)
    {
        printf("Error: no se pudo cargar la imagen\n");
        return 1;
    }

    printf("Imagen cargada: %dx%d\n", ancho, altura);

    int cantidad = ancho * altura;

    // Convertir escala de grises a un buffer RGB (3 canales) requerido por harris_detect
    unsigned char *rgb = (unsigned char*) malloc(cantidad * 3);

    // Reservar memoria en CPU para procesamiento float
    float *entrada = (float*) malloc(cantidad * sizeof(float));
    float *gauss   = (float*) malloc(cantidad * sizeof(float));
    float *Ix      = (float*) malloc(cantidad * sizeof(float));
    float *Iy      = (float*) malloc(cantidad * sizeof(float));

    if (entrada == NULL || gauss == NULL || Ix == NULL || Iy == NULL || rgb == NULL)
    {
        printf("Error reservando memoria\n");
        stbi_image_free(imagen);
        free(entrada);
        free(gauss);
        free(Ix);
        free(Iy);
        free(rgb);
        return 1;
    }

    // Copiar datos de 1 canal a float y al arreglo RGB
    for (int i = 0; i < cantidad; i++)
    {
        unsigned char val = imagen[i];
        
        entrada[i] = (float)val;

        rgb[i * 3 + 0] = val; // R
        rgb[i * 3 + 1] = val; // G
        rgb[i * 3 + 2] = val; // B
    }

    // Liberar la imagen original leída por STB
    stbi_image_free(imagen);

    // 2. Filtro Gaussiano
    printf("Aplicando filtro Gaussiano\n");
    gaussian_filter_cuda(
        gauss,
        entrada,
        ancho,
        altura
    );

    // 3. Filtro Sobel
    printf("Aplicando filtro Sobel\n");
    sobel_filter_cuda(
        Ix,
        Iy,
        entrada,
        ancho,
        altura
    );

    // 4. Detector de Esquinas Harris
    printf("Aplicando Harris\n");

    /* El threshold de Harris depende de la escala de los gradientes,
      productos y filtros utilizados. Se usa un valor inicial alto
      y harris_detect muestra el rango real de R para poder ajustarlo.*/

    //Lo puse asi para que solo salgan los mas importantes
    float threshold = 1000000.0f;

    //Maximo de puntos
    int max_points = 10000;
    int *points_x = (int*) malloc(sizeof(int) * max_points);
    int *points_y = (int*) malloc(sizeof(int) * max_points);

    if (points_x != NULL && points_y != NULL)
    {
        harris_detect(
            rgb, 
            ancho, 
            altura, 
            threshold, 
            max_points, 
            points_x, 
            points_y
        );
    }
    else
    {
        printf("Error reservando memoria para los puntos Harris\n");
    }

    // 5. Convertir resultados de Gauss y Sobel a unsigned char para exportar
    unsigned char *gauss_img   = (unsigned char*) malloc(cantidad);
    unsigned char *sobel_x_img = (unsigned char*) malloc(cantidad);
    unsigned char *sobel_y_img = (unsigned char*) malloc(cantidad);

    if (gauss_img == NULL || sobel_x_img == NULL || sobel_y_img == NULL)
    {
        printf("Error reservando memoria para imagenes de salida\n");

        free(entrada);
        free(gauss);
        free(Ix);
        free(Iy);
        free(rgb);
        free(points_x);
        free(points_y);
        free(gauss_img);
        free(sobel_x_img);
        free(sobel_y_img);

        return 1;
    }

    for (int i = 0; i < cantidad; i++)
    {
        // Gauss
        float valor = gauss[i];
        if (valor < 0)   valor = 0;
        if (valor > 255) valor = 255;
        gauss_img[i] = (unsigned char)valor;

        // Sobel GX
        valor = Ix[i];
        valor = valor < 0 ? -valor : valor;
        if (valor > 255) valor = 255;
        sobel_x_img[i] = (unsigned char)valor;

        // Sobel GY
        valor = Iy[i];
        valor = valor < 0 ? -valor : valor;
        if (valor > 255) valor = 255;
        sobel_y_img[i] = (unsigned char)valor;
    }

    // 6. Guardar imágenes generadas

    //No ocupo lo de sobel o gauss
    /*stbi_write_png(
        "images/gauss.png",
        ancho,
        altura,
        1,
        gauss_img,
        ancho
    );*/

    /*stbi_write_png(
        "images/sobel_x.png",
        ancho,
        altura,
        1,
        sobel_x_img,
        ancho
    );*/

    /*stbi_write_png(
        "images/sobel_y.png",
        ancho,
        altura,
        1,
        sobel_y_img,
        ancho
    );*/

    // Guardar resultado Harris (3 canales para ver las marcas rojas)
    // Se cambia la extension a .png porque se usa stbi_write_png.
    stbi_write_png(
        "images/resultado_harris.png",
        ancho,
        altura,
        3,
        rgb,
        ancho * 3
    );

    printf("Fin.\n");

    // 7. Liberar memoria
    free(entrada);
    free(gauss);
    free(Ix);
    free(Iy);
    free(rgb);
    free(points_x);
    free(points_y);
    free(gauss_img);
    free(sobel_x_img);
    free(sobel_y_img);

    return 0;
}