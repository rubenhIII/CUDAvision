#include <stdio.h>
#include <stdlib.h>
#include "match_visualization.h"
#include "../stb_image_write.h"

// Dibuja un pixel en una imagen RGB
void dibujar_pixel(
    unsigned char* imagen,
    int ancho,
    int altura,
    int x,
    int y,
    unsigned char r,
    unsigned char g,
    unsigned char b)
{
    if (x < 0 || x >= ancho || y < 0 || y >= altura)
        return;

    int posicion = (y * ancho + x) * 3;

    imagen[posicion] = r;
    imagen[posicion + 1] = g;
    imagen[posicion + 2] = b;
}


// Dibuja una linea entre dos puntos
void dibujar_linea(
    unsigned char* imagen,
    int ancho,
    int altura,
    int x1,
    int y1,
    int x2,
    int y2,
    unsigned char r,
    unsigned char g,
    unsigned char b)
{
    int dx = abs(x2 - x1);
    int dy = abs(y2 - y1);

    int sx = x1 < x2 ? 1 : -1;
    int sy = y1 < y2 ? 1 : -1;

    int err = dx - dy;

    while (1)
    {
        dibujar_pixel(
            imagen,
            ancho,
            altura,
            x1,
            y1,
            r,
            g,
            b
        );

        if (x1 == x2 && y1 == y2)
            break;

        int e2 = 2 * err;

        if (e2 > -dy)
        {
            err -= dy;
            x1 += sx;
        }

        if (e2 < dx)
        {
            err += dx;
            y1 += sy;
        }
    }
}


// Dibuja un punto como un pequeño circulo
void dibujar_punto(
    unsigned char* imagen,
    int ancho,
    int altura,
    int x,
    int y,
    int radio,
    unsigned char r,
    unsigned char g,
    unsigned char b)
{
    for (int dy = -radio; dy <= radio; dy++)
    {
        for (int dx = -radio; dx <= radio; dx++)
        {
            if (dx * dx + dy * dy <= radio * radio)
            {
                dibujar_pixel(
                    imagen,
                    ancho,
                    altura,
                    x + dx,
                    y + dy,
                    r,
                    g,
                    b
                );
            }
        }
    }
}


// Crea una imagen comparando las dos imagenes
// mediante lineas entre los matches
void crear_comparacion_matches(
    unsigned char* imagen1,
    unsigned char* imagen2,
    int ancho,
    int altura,
    int* points_x,
    int* points_y,
    int* points_x2,
    int* points_y2,
    unsigned int** matches,
    int matching_points)
{
    int ancho_total = ancho * 2;

    unsigned char* comparacion =
        (unsigned char*)malloc(
            (size_t)ancho_total * altura * 3
        );

    if (!comparacion)
    {
        printf(
            "Error reservando memoria para imagen de comparacion.\n"
        );

        return;
    }

    // Copiar imagen 1 e imagen 2 lado a lado

    for (int y = 0; y < altura; y++)
    {
        for (int x = 0; x < ancho; x++)
        {
            int origen = (y * ancho + x) * 3;

            int destino1 =
                (y * ancho_total + x) * 3;

            int destino2 =
                (y * ancho_total + ancho + x) * 3;

            unsigned char gris1 =
                (unsigned char)(
                    0.299 * imagen1[origen] +
                    0.587 * imagen1[origen + 1] +
                    0.114 * imagen1[origen + 2]
                );

            unsigned char gris2 =
                (unsigned char)(
                    0.299 * imagen2[origen] +
                    0.587 * imagen2[origen + 1] +
                    0.114 * imagen2[origen + 2]
                );

            comparacion[destino1] =
                gris1;

            comparacion[destino1 + 1] =
                gris1;

            comparacion[destino1 + 2] =
                gris1;

            comparacion[destino2] =
                gris2;

            comparacion[destino2 + 1] =
                gris2;

            comparacion[destino2 + 2] =
                gris2;
        }
    }


    // Dibujar lineas entre los matches

    for (int i = 0; i < matching_points; i++)
    {
        int punto1 = matches[i][0];
        int punto2 = matches[i][1];

        int x1 = points_x[punto1];
        int y1 = points_y[punto1];

        int x2 = points_x2[punto2];
        int y2 = points_y2[punto2];

        // La segunda imagen esta a la derecha
        x2 += ancho;

        // Dibujar linea del match
        dibujar_linea(
            comparacion,
            ancho_total,
            altura,
            x1,
            y1,
            x2,
            y2,
            255,
            0,
            0
        );

        // Dibujar punto en la imagen original
        dibujar_punto(
            comparacion,
            ancho_total,
            altura,
            x1,
            y1,
            4,
            0,
            255,
            0
        );

        // Dibujar punto en la imagen trasladada
        dibujar_punto(
            comparacion,
            ancho_total,
            altura,
            x2,
            y2,
            4,
            0,
            255,
            0
        );
    }

    // --------------------------------------------------
    // Guardar imagen de comparacion
    // --------------------------------------------------

    if (!stbi_write_png(
        "images/comparacion_matches.png",
        ancho_total,
        altura,
        3,
        comparacion,
        ancho_total * 3))
    {
        printf(
            "Error guardando imagen de comparacion.\n"
        );
    }
    else
    {
        printf(
            "Imagen de comparacion guardada correctamente.\n"
        );
    }

    free(comparacion);
}