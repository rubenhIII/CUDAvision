#ifndef IMAGE_PROCESS_H
#define IMAGE_PROCESS_H


//Padding
/* 
    image_padding

   param image   Vector RGB de entrada  (width * height * 3 bytes)
   param width   Ancho original en pixeles
   param height  Alto original en pixeles
   param pad     Numero de pixeles de padding a agregar en cada borde
   return        Vector RGB de (width+2*pad) * (height+2*pad) * 3 bytes,
                 o NULL en caso de error.
 */
unsigned char* image_padding(unsigned char *image, int width, int height, int pad);

//Filtro sobel

/*
  sobel_filter
 
  param image   Vector RGB de entrada  (width * height * 3 bytes)
  param width   Ancho en pixeles
  param height  Alto en pixeles
  return        Vector RGB de width * height * 3 bytes con los bordes detectados,
                 o NULL en caso de error.
 */
unsigned char* sobel_filter(unsigned char *image, int width, int height);

//Filtro Gaussiano

/*
 
    Kernel:
    [ 1  2  1 ]
    [ 2  4  2 ]  /  16
    [ 1  2  1 ]
 
  param image   Vector RGB de entrada  (width * height * 3 bytes)
  param width   Ancho en pixeles
  param height  Alto en pixeles
  return        Vector RGB de width * height * 3 bytes con la imagen suavizada,
                 o NULL en caso de error.
*/
unsigned char* gaussian_filter(unsigned char *image, int width, int height);


#endif 




