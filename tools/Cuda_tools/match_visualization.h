#ifndef MATCH_VISUALIZATION_H
#define MATCH_VISUALIZATION_H

#ifdef __cplusplus
extern "C" {
#endif

void dibujar_pixel(
    unsigned char* imagen,
    int ancho,
    int altura,
    int x,
    int y,
    unsigned char r,
    unsigned char g,
    unsigned char b
);

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
    unsigned char b
);

void dibujar_punto(
    unsigned char* imagen,
    int ancho,
    int altura,
    int x,
    int y,
    int radio,
    unsigned char r,
    unsigned char g,
    unsigned char b
);

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
    int matching_points
);

#ifdef __cplusplus
}
#endif

#endif