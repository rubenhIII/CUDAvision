/*
  Genera una imagen  con un cuadro rojo dividido en secciones (filas x columnas).
 Cada sección puede ser roja o gris según el arreglo `secciones_grises[]`.

Compilar:
 gcc cuadro_segmentado.c -o cuadro_segmentado -lm

 */

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

#include <stdio.h>
#include <string.h>

//Variables principales

//Ancho de la imagen de fondo (base)
#define IMAGEN_ANCHO   512   //pixeles de la imagen
#define IMAGEN_ALTO    512

#define CUADRO_X       64    //Empezamos en 64 pixeles con respecto al lienzo base
#define CUADRO_Y       64    //Para dibujar nuestro cuadro rojo

    //Definimos 384 puesto que al tomar 64 de margen haciendo la resta tanto de ancho como alto
    //512-64-64 = 384
#define CUADRO_ANCHO   384   //Tamaño de el cuadro a dibujar
#define CUADRO_ALTO    384  //En el lienzo de 512*512

#define COLS           4     //Numero de columnas a segmentar
#define FILAS          4     //Numero de filas a segmentar el cuadro

#define GROSOR_BORDE   1     //Pixeles de borde entre secciones

//Colores (R G B)
//Definimos los colores a usar
#define COLOR_FONDO_R  30,  30,  30   // fondo de la imagen
#define COLOR_ROJO_R   220, 40,  40   //coloreado de secciones rojas
#define COLOR_GRIS_R   160, 160, 160  //coloreado de secciones grises
#define COLOR_BORDE_R  20,  20,  20   //Lineas divisoras del borde

//Mapa de secciones grises
/*
  Tabla de FILAS × COLS.  
  1 va a ser gris
  0 va a ser rojo
 
  Fila 0 es la fila superior; columna 0 es la columna izquierda.
 */
static const int secciones_grises[FILAS][COLS] = {
        //   Col:  0  1  2  3
    /* Fila 0 */ { 0, 0, 0, 0 },
    /* Fila 1 */ { 0, 0, 0, 0 },
    /* Fila 2 */ { 0, 0, 0, 0 },
    /* Fila 3 */ { 0, 0, 0, 0 },
};


//Funcion principal

int main(void)
{
    //El stride es el tamaño de bytes for fila
    //Se calcula con ancho*canales
    const int stride = IMAGEN_ANCHO * 3;
    //Guardamos memoria
    unsigned char *img = (unsigned char *)calloc(IMAGEN_ALTO * stride, 1);
    if (!img) { fprintf(stderr, "Sin memoria\n"); return 1; }

    //Rellenamos el fondo de 512*512
    for (int y = 0; y < IMAGEN_ALTO; y++) {
        for (int x = 0; x < IMAGEN_ANCHO; x++) {
            unsigned char *p = img + y * stride + x * 3;
            p[0] = 30; p[1] = 30; p[2] = 30;   /* COLOR_FONDO */
        }
    }

    //Dibujas las secciones del cuadro
    int sec_ancho = CUADRO_ANCHO / COLS;
    int sec_alto  = CUADRO_ALTO  / FILAS;

    for (int fila = 0; fila < FILAS; fila++) {
        for (int col = 0; col < COLS; col++) {

            /* Coordenadas en píxeles de esta sección */
            int x0 = CUADRO_X + col  * sec_ancho + GROSOR_BORDE;
            int y0 = CUADRO_Y + fila * sec_alto  + GROSOR_BORDE;
            int x1 = CUADRO_X + (col  + 1) * sec_ancho - GROSOR_BORDE;
            int y1 = CUADRO_Y + (fila + 1) * sec_alto  - GROSOR_BORDE;

            /* Color según el mapa */
            unsigned char r, g, b;
            if (secciones_grises[fila][col]) {
                r = 160; g = 160; b = 160;  /* GRIS */
            } else {
                r = 220; g =  40; b =  40;  /* ROJO */
            }

            /* Pinta píxel a píxel */
            for (int py = y0; py < y1; py++) {
                for (int px = x0; px < x1; px++) {
                    if (px < 0 || px >= IMAGEN_ANCHO) continue;
                    if (py < 0 || py >= IMAGEN_ALTO)  continue;
                    unsigned char *p = img + py * stride + px * 3;
                    p[0] = r; p[1] = g; p[2] = b;
                }
            }
        }
    }

    //guarda la imagen
    const char *nombre = "salida.png";
    if (!stbi_write_png(nombre, IMAGEN_ANCHO, IMAGEN_ALTO, 3, img, stride)) {
        fprintf(stderr, "Error al escribir %s\n", nombre);
        free(img);
        return 1;
    }

    printf("Imagen generada: %s\n", nombre);
    printf("Cuadro: %dx%d px, dividido en %d filas x %d columnas\n",
           CUADRO_ANCHO, CUADRO_ALTO, FILAS, COLS);
    printf("Secciones grises marcadas con 1 en `secciones_grises[][]`.\n");

    free(img);
    return 0;
}