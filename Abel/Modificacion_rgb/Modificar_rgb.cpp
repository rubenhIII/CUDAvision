#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

#include <stdio.h>
#include <stdlib.h>


//funcion modificar pixeles
/*
    En una imagen de 4*4 pixeles cada pixel tiene 3 bytes, rgb
columnas         0       1       2       3
 Fila 0       [R G B] [R G B] [R G B] [R G B]
 Fila 1       [R G B] [R G B] [R G B] [R G B]
 Fila 2       [R G B] [R G B] [R G B] [R G B]
 Fila 3       [R G B] [R G B] [R G B] [R G B]
 Imagen con 16 pixeles

    para acceder a un byte en especifico seria
    (num_fila*ancho_imagen+ columna) para acceder al pixel
    y para tomar en cuenta los bytes multiplicamos todo por los canales
    (num_fila*ancho_imagen+ columna)*canales
    Esto solo te pondria al inicio del pixel
    en total son 48 bytes si quisiera acceder al byte 47
    seria
    (i*ancho + j)*canales 
    (3*4 + 3)*3 esto nos deja en el byte 45 al "inicio" del pixel 16
    Ahi le sumamos la posicion dependiendo de cual querramos

    Posiciones
    [R G B]
    1  2 3

    Al final la formula seria
    (i*ancho + j)*canales + posicion_byte
    y quedaria:
    (3*4 + 3)*3 + 2
    siendo el byte 47

    Ahora para extraer los bytes R, G, B por separado.
    
    Dado el arrgelo de 1D.
    Todos los bytes R empiezan en 0 y se le suma 3
    0 3 6 9 12 15 18 21
    con modulo de 3 se saca 0

    Para los bytes G se suma 1
    Empiezan en 1 4 7 10 13 16 19
    Igual usando modulo con residuo de 1

    Y para B se suma 2
    2 5 8 11 14 17 20
    Residuo de 2

*/
void Extraer_canal(unsigned char *pixeles, int ancho, int alto, int canales,
                    unsigned char *destinoR, unsigned char *destinoG, unsigned char 
                    *destinoB){
    int indice_pixel = 0;
    //pixeles tiene todos los bytes juntos queremos separarlo
    for(int i = 0; i < ancho*alto*canales; i++){
        //Canal R
        if(i%canales==0){
            destinoR[indice_pixel]=pixeles[i];
        }
        //Canal G
        if(i%canales==1){
            destinoG[indice_pixel]=pixeles[i];
        }
        //Canal B
        if(i%canales==2){
            destinoB[indice_pixel]=pixeles[i];
            indice_pixel++; //Avanzamos al siguiente pixel pues B es el ultimo
        }
    }
}
//Funcion para hacer copias de la imagen-crear
void hacerCopia(unsigned char *pixeles, int ancho, int alto, int canales,  const char *nombre_guardado){
    //Guardamos copia de la imagen para verificar
    const char *salida = nombre_guardado;
    //Stride es la cantidad de bytes por fila
    int stride = ancho * canales;
    if (stbi_write_jpg(salida, ancho, alto, canales, pixeles, stride)) {
        printf("\nCopia guardada en: %s\n", salida);
    } else {
        fprintf(stderr, "Error al guardar la copia.\n");
    }
}

void Reconstruccion(unsigned char *canalR, unsigned char *canalG, 
                    unsigned char *canalB, int ancho, int alto, int canales, 
                    unsigned char *destino){
    //Nos movemos por pixeles pues los arrays de rgb los guardamos en pixeles
    //1 byte por 1 pixel 
    for(int i = 0; i < ancho*alto;i++){

        //El indice va a imprimir los bytes
        destino[i*canales] = canalR[i];
        destino[i*canales +1] = canalG[i];
        destino[i*canales +2] = canalB[i];
        
    }
}
void Potencia(unsigned char *canalR, unsigned char *canalG, 
                    unsigned char *canalB, int ancho, int alto, int canales, 
                    unsigned char *destino){   
    //Nos movemos por pixeles pues los arrays de rgb los guardamos en pixeles
    //1 byte por 1 pixel 
    int valorR, valorG, valorB;
    for(int i = 0; i < ancho*alto;i++){
        //Potencia
        valorR= canalR[i]*1.5;
        valorG= canalG[i]*1.5;
        valorB= canalB[i]*1.5;
        //Evitar que sobrepase 255
        if(valorR>255)valorR=255;
        if(valorG>255)valorG=255;
        if(valorB>255)valorB=255;

        //El indice va a imprimir los bytes
        destino[i*canales] = valorR;
        destino[i*canales +1] = valorG;
        destino[i*canales +2] = valorB;
    }
}

void Promedio(unsigned char *canalR, unsigned char *canalG, 
                    unsigned char *canalB, int ancho, int alto, int canales, 
                    unsigned char *destino){
    //Nos movemos por pixeles pues los arrays de rgb los guardamos en pixeles
    //1 byte por 1 pixel 

    for(int i = 0; i < ancho*alto;i++){
        unsigned char promedio = (canalR[i] + canalG[i] + canalB[i])/3;
        //El indice va a imprimir los bytes
        //Nos movemos por i*canales para ir moviendonos de pixel en pixel
        destino[i*canales] = promedio;
        destino[i*canales +1] = promedio;
        destino[i*canales +2] = promedio;
        
    }
}


void swap(unsigned char *canalR, unsigned char *canalG, 
                    unsigned char *canalB, int ancho, int alto, int canales, 
                    unsigned char *destino){
    //Nos movemos por pixeles pues los arrays de rgb los guardamos en pixeles
    //1 byte por 1 pixel 

    for(int i = 0; i < ancho*alto;i++){
        
        //Vamos a reordenar para que ahora sea BRG
        destino[i*canales] = canalB[i];
        destino[i*canales +1] = canalR[i];
        destino[i*canales +2] = canalG[i];
        
    }
}


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
 
 
 //Usamos la funcion de la libreria stbi load
    //Va a guardar el alto, ancho y canales en un apuntador
    unsigned char *pixeles = stbi_load(ruta, &ancho, &alto, &canales, 0);
    //Por si hay algun error
    if (!pixeles) {
        fprintf(stderr, "Error al leer '%s': %s\n", ruta, stbi_failure_reason());
        return 1;
    }

    //Copias de nuestro array de pixeles original para hacer modificaciones
    unsigned char *Pixeles2 = (unsigned char*) malloc (ancho*alto*canales*sizeof(unsigned char));
    unsigned char *PixelesPotencia = (unsigned char*) malloc (ancho*alto*canales*sizeof(unsigned char));
    unsigned char *PixelesPromedio = (unsigned char*) malloc (ancho*alto*canales*sizeof(unsigned char));
    unsigned char *PixelesSwap = (unsigned char*) malloc (ancho*alto*canales*sizeof(unsigned char));

    //Hacemos arreglos para guardar solamente un byte de cada pixel
    //Es decir tiene el tamaño en pixeles y no en bytes
    unsigned char *PixelesR = (unsigned char*) malloc (ancho*alto*sizeof(unsigned char));
    unsigned char *PixelesG = (unsigned char*) malloc (ancho*alto*sizeof(unsigned char));
    unsigned char *PixelesB = (unsigned char*) malloc (ancho*alto*sizeof(unsigned char));
    //Modificamos
    Extraer_canal(pixeles, ancho, alto, canales, PixelesR, PixelesG, PixelesB);

    //Separacion en 3 canales
    //Solo usamos 1 canal pues solo guardamos un byte
    hacerCopia(PixelesR, ancho, alto, 1, "OnlyRed.jpeg");

    hacerCopia(PixelesG, ancho, alto, 1, "OnlyGreen.jpeg");

    hacerCopia(PixelesB, ancho, alto, 1, "OnlyBlue.jpeg");

    //Vamos a hacer modificaciones

    swap(PixelesR, PixelesG, PixelesB, ancho, alto, canales, PixelesSwap);
    hacerCopia(PixelesSwap, ancho, alto, canales, "Swap.jpeg");

    Potencia(PixelesR, PixelesG, PixelesB, ancho, alto, canales, PixelesPotencia);
    hacerCopia(PixelesPotencia, ancho, alto, canales, "Potencia.jpeg");

    Promedio(PixelesR, PixelesG, PixelesB, ancho, alto, canales, PixelesPromedio);
    hacerCopia(PixelesPromedio, ancho, alto, canales, "Promedio.jpeg");

    //Reconstruimos la imagen original
    Reconstruccion(PixelesR, PixelesG, PixelesB, ancho, alto, canales, Pixeles2);
    
    hacerCopia(Pixeles2, ancho, alto, canales, "reconstruccion.jpeg");

    //Liberar memoria
    stbi_image_free(pixeles);
    free(Pixeles2);
    free(PixelesB);
    free(PixelesG);
    free(PixelesR);
    free(PixelesSwap);
    free(PixelesPotencia);
    free(PixelesPromedio);


}


