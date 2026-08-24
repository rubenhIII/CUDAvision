#include <stdio.h>
#include <stdlib.h>
#define STB_IMAGE_IMPLEMENTATION
#include "../stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "../stb_image_write.h"
#include "image_process_cuda.h"



int main()
{
    //Parámetros de la imagen
    int ancho;
    int altura;
    int canales;


    // 1. Cargar imagen

   //Para imágenes se usa unsigned char, para cálculos float

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


    // Reservamos memoria en CPU


    int cantidad = ancho * altura;

    float *entrada = (float*)malloc(
        cantidad * sizeof(float)
    );

    float *gauss = (float*)malloc(
        cantidad * sizeof(float)
    );

    float *Ix = (float*)malloc(
        cantidad * sizeof(float)
    );

    float *Iy = (float*)malloc(
        cantidad * sizeof(float)
    );

    if (entrada == NULL ||
        gauss == NULL ||
        Ix == NULL ||
        Iy == NULL)
    {
        printf("Error reservando memoria\n");

        stbi_image_free(imagen);

        free(entrada);
        free(gauss);
        free(Ix);
        free(Iy);

        return 1;
    }



    // Convertir unsigned char a float


    for (int i = 0; i < cantidad; i++)
    {
        entrada[i] = (float)imagen[i];
    }



    stbi_image_free(imagen);



    // Filtro Gaussiano


    printf("Aplicando filtro Gaussiano\n");

    gaussian_filter_cuda(
        gauss,
        entrada,
        ancho,
        altura
    );



    // Filtro Sobel


    printf("Aplicando filtro Sobel\n");

    sobel_filter_cuda(
        Ix,
        Iy,
        entrada,
        ancho,
        altura
    );



    // Convertir resultados a unsigned char


    unsigned char *gauss_img =
        (unsigned char*)malloc(cantidad);

    unsigned char *sobel_x_img =
        (unsigned char*)malloc(cantidad);

    unsigned char *sobel_y_img =
        (unsigned char*)malloc(cantidad);


    for (int i = 0; i < cantidad; i++)
    {
        
        // Gauss
        

        float valor = gauss[i];

        if (valor < 0)
            valor = 0;

        if (valor > 255)
            valor = 255;

        gauss_img[i] = (unsigned char)valor;


        
        // Sobel GX
        

        valor = Ix[i];

        // Sobel puede producir valores negativos
        valor = valor < 0 ? -valor : valor;

        if (valor > 255)
            valor = 255;

        sobel_x_img[i] = (unsigned char)valor;



        // Sobel GY
        

        valor = Iy[i];

        valor = valor < 0 ? -valor : valor;

        if (valor > 255)
            valor = 255;

        sobel_y_img[i] = (unsigned char)valor;
    }


    //Guardamos imagenes

    stbi_write_png(
        "images/gauss.png",
        ancho,
        altura,
        1,
        gauss_img,
        ancho
    );

    stbi_write_png(
        "images/sobel_x.png",
        ancho,
        altura,
        1,
        sobel_x_img,
        ancho
    );

    stbi_write_png(
        "images/sobel_y.png",
        ancho,
        altura,
        1,
        sobel_y_img,
        ancho
    );


    // Liberamos memoria

    free(entrada);
    free(gauss);
    free(Ix);
    free(Iy);

    free(gauss_img);
    free(sobel_x_img);
    free(sobel_y_img);

    return 0;
}