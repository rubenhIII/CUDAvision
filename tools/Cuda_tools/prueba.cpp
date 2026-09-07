#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define STB_IMAGE_IMPLEMENTATION
#include "../stb_image.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "../stb_image_write.h"

#include "image_process_cuda.h"
#include "harris_cuda.h"
#include "image_descriptor_cuda.h"
#include "match_visualization.c"


// Traslada una imagen hacia abajo
void trasladar_abajo(
    unsigned char* origen,
    unsigned char* destino,
    int ancho,
    int altura,
    int canales,
    int desplazamiento)
{
    // Inicializar toda la imagen trasladada en negro
    memset(
        destino,
        0,
        (size_t)ancho * altura * canales
    );

    // Copiar la imagen desplazada hacia abajo
    for (int y = desplazamiento; y < altura; y++)
    {
        int y_origen = y - desplazamiento;

        memcpy(
            destino + (size_t)y * ancho * canales,
            origen + (size_t)y_origen * ancho * canales,
            (size_t)ancho * canales
        );
    }
}


// Reserva una matriz de descriptores
unsigned char** reservar_descriptores(
    int num_points,
    int descriptor_len)
{
    unsigned char** descriptors =
        (unsigned char**)malloc(
            sizeof(unsigned char*) * num_points
        );

    if (!descriptors)
        return NULL;

    for (int i = 0; i < num_points; i++)
    {
        descriptors[i] =
            (unsigned char*)malloc(
                sizeof(unsigned char) * descriptor_len
            );

        if (!descriptors[i])
        {
            for (int j = 0; j < i; j++)
                free(descriptors[j]);

            free(descriptors);

            return NULL;
        }
    }

    return descriptors;
}


// Libera una matriz de descriptores
void liberar_descriptores(
    unsigned char** descriptors,
    int num_points)
{
    if (!descriptors)
        return;

    for (int i = 0; i < num_points; i++)
        free(descriptors[i]);

    free(descriptors);
}


// Reserva el patrón BRIEF
double** reservar_patron(int descriptor_len)
{
    double** patron =
        (double**)malloc(
            sizeof(double*) * descriptor_len
        );

    if (!patron)
        return NULL;

    for (int i = 0; i < descriptor_len; i++)
    {
        patron[i] =
            (double*)malloc(
                sizeof(double) * 2
            );

        if (!patron[i])
        {
            for (int j = 0; j < i; j++)
                free(patron[j]);

            free(patron);

            return NULL;
        }
    }

    return patron;
}


// Libera el patrón BRIEF
void liberar_patron(
    double** patron,
    int descriptor_len)
{
    if (!patron)
        return;

    for (int i = 0; i < descriptor_len; i++)
        free(patron[i]);

    free(patron);
}

//MAIN
int main()
{
    //Semilla igual
    srand(12345);
    const char* nombre_imagen = "imagen.jpg";

    int ancho, altura, canales;

    // Cargar imagen
    unsigned char* imagen = stbi_load(
        nombre_imagen,
        &ancho,
        &altura,
        &canales,
        3
    );

    if (!imagen)
    {
        printf("Error al cargar la imagen.\n");
        return 1;
    }

    canales = 3;

    printf("Imagen cargada: %dx%d\n", ancho, altura);

    int size = ancho * altura * canales;

    // --------------------------------------------------
    // Crear copia RGB para Harris
    // --------------------------------------------------

    unsigned char* rgb =
        (unsigned char*)malloc(size);

    if (!rgb)
    {
        printf("Error reservando memoria para RGB.\n");

        stbi_image_free(imagen);

        return 1;
    }

    memcpy(
        rgb,
        imagen,
        size
    );

  
    // Crear imagen trasladada hacia abajo para comparar

    int desplazamiento = 50;

    unsigned char* imagen_trasladada =
        (unsigned char*)malloc(size);

    if (!imagen_trasladada)
    {
        printf("Error reservando memoria para imagen trasladada.\n");

        free(rgb);
        stbi_image_free(imagen);

        return 1;
    }

    trasladar_abajo(
        imagen,
        imagen_trasladada,
        ancho,
        altura,
        canales,
        desplazamiento
    );

    printf(
        "Imagen trasladada %d pixeles hacia abajo.\n",
        desplazamiento
    );


    // Crear copia para Harris de la segunda imagen

    unsigned char* rgb_trasladada =
        (unsigned char*)malloc(size);

    if (!rgb_trasladada)
    {
        printf(
            "Error reservando memoria para RGB trasladado.\n"
        );

        free(imagen_trasladada);
        free(rgb);
        stbi_image_free(imagen);

        return 1;
    }

    memcpy(
        rgb_trasladada,
        imagen_trasladada,
        size
    );


    // Guardar imagen trasladada


    if (!stbi_write_png(
        "images/imagen_trasladada.png",
        ancho,
        altura,
        3,
        imagen_trasladada,
        ancho * 3))
    {
        printf("Error guardando imagen trasladada.\n");
    }
    else
    {
        printf(
            "Imagen trasladada guardada correctamente.\n"
        );
    }


    // Harris

    int max_points = 10000;

    int* points_x =
        (int*)malloc(
            sizeof(int) * max_points
        );

    int* points_y =
        (int*)malloc(
            sizeof(int) * max_points
        );
//Imagen 2
    int* points_x2 =
        (int*)malloc(
            sizeof(int) * max_points
        );

    int* points_y2 =
        (int*)malloc(
            sizeof(int) * max_points
        );
        //Validacion
    if (!points_x ||
        !points_y ||
        !points_x2 ||
        !points_y2)
    {
        printf(
            "Error reservando memoria para puntos Harris.\n"
        );

        if (points_x) free(points_x);
        if (points_y) free(points_y);
        if (points_x2) free(points_x2);
        if (points_y2) free(points_y2);

        free(rgb_trasladada);
        free(imagen_trasladada);
        free(rgb);

        stbi_image_free(imagen);

        return 1;
    }

    float threshold = 1000000.0f;

    unsigned char* imagen_gris_brief =
        (unsigned char*)malloc(size);

    unsigned char* imagen_gris_brief_trasladada =
        (unsigned char*)malloc(size);

    if (!imagen_gris_brief ||
        !imagen_gris_brief_trasladada)
    {
        printf(
            "Error reservando memoria para imagenes en gris de BRIEF.\n"
        );

        if (imagen_gris_brief)
            free(imagen_gris_brief);

        if (imagen_gris_brief_trasladada)
            free(imagen_gris_brief_trasladada);

        free(points_x);
        free(points_y);
        free(points_x2);
        free(points_y2);

        free(rgb_trasladada);
        free(imagen_trasladada);
        free(rgb);

        stbi_image_free(imagen);

        return 1;
    }

    // Harris imagen original

    printf("\n===== HARRIS IMAGEN 1 =====\n");

    int num_points =
        harris_detect(
            rgb,
            ancho,
            altura,
            threshold,
            max_points,
            points_x,
            points_y,
            imagen_gris_brief
        );

    printf(
        "Cantidad real de puntos Harris imagen 1: %d\n",
        num_points
    );


    // Harris imagen trasladada
    printf("\n===== HARRIS IMAGEN 2 =====\n");

    int num_points2 =
        harris_detect(
            rgb_trasladada,
            ancho,
            altura,
            threshold,
            max_points,
            points_x2,
            points_y2,
            imagen_gris_brief_trasladada
        );

    printf(
        "Cantidad real de puntos Harris imagen 2: %d\n",
        num_points2
    );

    // --------------------------------------------------
    // Guardar resultados Harris
    // --------------------------------------------------

    if (!stbi_write_png(
        "images/resultado_harris.png",
        ancho,
        altura,
        3,
        rgb,
        ancho * 3))
    {
        printf(
            "Error guardando resultado Harris imagen 1.\n"
        );
    }
    else
    {
        printf(
            "Resultado Harris imagen 1 guardado.\n"
        );
    }

    if (!stbi_write_png(
        "images/resultado_harris_trasladada.png",
        ancho,
        altura,
        3,
        rgb_trasladada,
        ancho * 3))
    {
        printf(
            "Error guardando resultado Harris imagen 2.\n"
        );
    }
    else
    {
        printf(
            "Resultado Harris imagen 2 guardado.\n"
        );
    }

    // --------------------------------------------------
    // BRIEF
    // --------------------------------------------------

    if (num_points > 0 && num_points2 > 0)
    {
        printf("\n===== BRIEF =====\n");

        int descriptor_len = 256;

        // Patrones de BRIEF
        double** norm_x =
            reservar_patron(descriptor_len);

        double** norm_y =
            reservar_patron(descriptor_len);

        if (!norm_x || !norm_y)
        {
            printf(
                "Error reservando memoria para patrones BRIEF.\n"
            );

            liberar_patron(
                norm_x,
                descriptor_len
            );

            liberar_patron(
                norm_y,
                descriptor_len
            );

            free(points_x);
            free(points_y);
            free(points_x2);
            free(points_y2);

            free(imagen_gris_brief);
            free(imagen_gris_brief_trasladada);

            free(rgb_trasladada);
            free(imagen_trasladada);
            free(rgb);

            stbi_image_free(imagen);

            return 1;
        }

        // Generar patrón BRIEF
        sample_pattern(
            10.0,
            norm_x,
            norm_y,
            descriptor_len
        );

        // --------------------------------------------------
        // Descriptores imagen 1
        // --------------------------------------------------

        unsigned char** descriptors =
            reservar_descriptores(
                num_points,
                descriptor_len
            );

        // --------------------------------------------------
        // Descriptores imagen 2
        // --------------------------------------------------

        unsigned char** descriptors2 =
            reservar_descriptores(
                num_points2,
                descriptor_len
            );

        if (!descriptors || !descriptors2)
        {
            printf(
                "Error reservando memoria para descriptores.\n"
            );

            liberar_descriptores(
                descriptors,
                num_points
            );

            liberar_descriptores(
                descriptors2,
                num_points2
            );

            liberar_patron(
                norm_x,
                descriptor_len
            );

            liberar_patron(
                norm_y,
                descriptor_len
            );

            free(points_x);
            free(points_y);
            free(points_x2);
            free(points_y2);

            free(imagen_gris_brief);
            free(imagen_gris_brief_trasladada);

            free(rgb_trasladada);
            free(imagen_trasladada);
            free(rgb);

            stbi_image_free(imagen);

            return 1;
        }

        // Calcular BRIEF imagen 1
        brief_descriptor(
            imagen_gris_brief,
            ancho,
            altura,
            descriptors,
            descriptor_len,
            points_x,
            points_y,
            num_points,
            norm_x,
            norm_y
        );

        // Calcular BRIEF imagen 2
        brief_descriptor(
            imagen_gris_brief_trasladada,
            ancho,
            altura,
            descriptors2,
            descriptor_len,
            points_x2,
            points_y2,
            num_points2,
            norm_x,
            norm_y
        );

        printf(
            "Descriptores BRIEF imagen 1: %d\n",
            num_points
        );

        printf(
            "Descriptores BRIEF imagen 2: %d\n",
            num_points2
        );

        printf(
            "Longitud de cada descriptor: %d bits\n",
            descriptor_len
        );

        // --------------------------------------------------
        // Hamming
        // --------------------------------------------------

        printf("\n===== HAMMING =====\n");

        /*
         * Hamming necesita que ambas imagenes tengan
         * la misma cantidad de descriptores.
         *
         * Se utiliza la cantidad menor para evitar
         * acceder fuera de los limites.
         */
        int matching_points =
            num_points < num_points2
            ? num_points
            : num_points2;

        printf(
            "Puntos utilizados para matching: %d\n",
            matching_points
        );

        unsigned int** matches =
            hamming(
                descriptors,
                descriptors2,
                descriptor_len,
                matching_points
            );

        if (!matches)
        {
            printf(
                "Error calculando matches.\n"
            );

            liberar_descriptores(
                descriptors,
                num_points
            );

            liberar_descriptores(
                descriptors2,
                num_points2
            );

            liberar_patron(
                norm_x,
                descriptor_len
            );

            liberar_patron(
                norm_y,
                descriptor_len
            );

            free(points_x);
            free(points_y);
            free(points_x2);
            free(points_y2);

            free(imagen_gris_brief);
            free(imagen_gris_brief_trasladada);

            free(rgb_trasladada);
            free(imagen_trasladada);
            free(rgb);

            stbi_image_free(imagen);

            return 1;
        }

        // Mostrar algunos resultados
        int mostrar = matching_points;

        if (mostrar > 20)
            mostrar = 20;

        for (int i = 0; i < mostrar; i++)
        {
            int punto1 = matches[i][0];
            int punto2 = matches[i][1];
            int distancia = matches[i][2];

            printf(
                "Punto %d (%d,%d) -> Match: %d (%d,%d), Distancia Hamming: %d\n",
                punto1,
                points_x[punto1],
                points_y[punto1],
                punto2,
                points_x2[punto2],
                points_y2[punto2],
                distancia
            );
        }

        // --------------------------------------------------
        // Crear imagen de comparacion
        // --------------------------------------------------

        crear_comparacion_matches(
            rgb,
            rgb_trasladada,
            ancho,
            altura,
            points_x,
            points_y,
            points_x2,
            points_y2,
            matches,
            matching_points
        );

        // --------------------------------------------------
        // Liberar BRIEF / Hamming
        // --------------------------------------------------

        for (int i = 0; i < matching_points; i++)
            free(matches[i]);

        free(matches);

        liberar_descriptores(
            descriptors,
            num_points
        );

        liberar_descriptores(
            descriptors2,
            num_points2
        );

        liberar_patron(
            norm_x,
            descriptor_len
        );

        liberar_patron(
            norm_y,
            descriptor_len
        );
    }
    else
    {
        printf(
            "\nNo se encontraron suficientes puntos Harris.\n"
        );

        printf(
            "No se ejecutara BRIEF ni Hamming.\n"
        );
    }

    // --------------------------------------------------
    // Liberar memoria
    // --------------------------------------------------

    free(points_x);
    free(points_y);

    free(points_x2);
    free(points_y2);

    free(imagen_gris_brief);

    free(imagen_gris_brief_trasladada);

    free(rgb_trasladada);

    free(imagen_trasladada);

    free(rgb);

    stbi_image_free(imagen);

    printf(
        "\nPrograma terminado correctamente.\n"
    );

    return 0;
}