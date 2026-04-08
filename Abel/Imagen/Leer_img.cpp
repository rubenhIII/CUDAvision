/*
 leer_imagen.c
 
 Lee una imagen (PNG, JPG, BMP, TGA...) con stb_image y muestra su info.
 También guarda una copia en output_copia.png para verificar que se leyó bien.
 
 Compilar:
   gcc leer_imagen.c -o leer_imagen 
 
 Ejecutar:
   ./leer_imagen mi_foto.png
 */

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

#include <stdio.h>
#include <stdlib.h>


//Funcion principal
int main(int argc, char *argv[])
{
    //Condicion para a la hora de ejecutar si o si pases 2 argumentos el exe y la imagen
    if (argc < 2) {
        printf("Uso: %s <archivo_imagen>\n", argv[0]);
        return 1;
    }
    //El primer argumento ingresado por medio de la terminal se guarda como ruta
    const char *ruta = argv[1];

    //Variables para leer imagen
    int ancho, alto, canales;

    /*
      stbi_load devuelve un arreglo unidimensional muy largo de píxeles en formato:
        [R G B] [R G B] [R G B]  ...  (de izquierda a derecha, arriba a abajo)

        
        
      El último parámetro (canales_deseados) fuerza el número de canales:
        0 = usar los que tenga la imagen
        1 = grises
        3 = forzar RGB
        4 = forzar RGBA
     */
    //Usamos la funcion de la libreria stbi load
    //Va a guardar el alto, ancho y canales en un apuntador
    unsigned char *pixeles = stbi_load(ruta, &ancho, &alto, &canales, 0);
    //Por si hay algun error
    if (!pixeles) {
        fprintf(stderr, "Error al leer '%s': %s\n", ruta, stbi_failure_reason());
        return 1;
    }

    //Informacion de la imagen
    printf("Imagen leída correctamente\n");
    printf("  Archivo: %s\n", ruta);
    printf("  Ancho  : %d px\n", ancho);
    printf("  Alto   : %d px\n", alto);
    printf("  Canales: %d  (%s)\n", canales,
           canales == 1 ? "Escala de grises" :
           canales == 2 ? "Gris + Alpha"     :
           canales == 3 ? "RGB"              :
           canales == 4 ? "RGBA"             : "otro");
    printf(" Tamaño en memoria: %d bytes\n", ancho * alto * canales);

//Leemos algunos pixeles de la imagen
    printf("\n Primeros 5 píxeles (fila 0) \n");
    /*
    pixeles apunta al inicio de toda la imagen.

    Como estás en la fila 0, el primer píxel está al inicio.

    pixel 0 empieza en pixeles + 0*canales
    pixel 1 empieza en pixeles + 1*canales
    pixel 2 empieza en pixeles + 2*canales
    Si es RGB:
    pixel 0 → bytes 0,1,2
    pixel 1 → bytes 3,4,5
    pixel 2 → bytes 6,7,8
    */
    for (int i = 0; i < 5 && i < ancho; i++) {
        unsigned char *p = pixeles + (i * canales);
        printf("  Píxel [0][%d]: ", i);
        if (canales >= 3) printf("R=%3d G=%3d B=%3d", p[0], p[1], p[2]);
        if (canales == 4) printf(" A=%3d", p[3]);
        if (canales == 1) printf("Gris=%3d", p[0]);
        printf("\n");
    }

    //Guardamos copia de la imagen para verificar
    const char *salida = "copia.jpeg";
    //Stride es la cantidad de bytes por fila
    int stride = ancho * canales;
    if (stbi_write_png(salida, ancho, alto, canales, pixeles, stride)) {
        printf("\nCopia guardada en: %s\n", salida);
    } else {
        fprintf(stderr, "Error al guardar la copia.\n");
    }

    //Liberar memoria
    stbi_image_free(pixeles);

    return 0;
}