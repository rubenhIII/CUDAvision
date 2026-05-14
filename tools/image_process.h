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

/*  Convolucion
 
  param out     Buffer de salida  (width * height floats)
  param in      Buffer de entrada (width * height floats)
  param width   Ancho en píxeles
  param height  Alto en píxeles
  param kernel  Kernel aplanado row-major, tamaño (2*half_k+1)²
  param half_k  Semitamaño del kernel (1 → 3×3, 2 → 5×5, …)
  param scale   Divisor de la suma acumulada (0 = sin división)
 */
void convolve_float(float *out, const float *in,
                    int width, int height,
                    const float *kernel, int half_k, float scale);


//Filtro sobel

/*
  sobel_filter
 
   param Ix      Gradiente horizontal (width * height floats)
   param Iy      Gradiente vertical   (width * height floats)
   param in      Buffer de entrada    (width * height floats)
   param width   Ancho en píxeles
   param height  Alto en píxeles
 */

//Float -> float
void sobel_filter(float *Ix, float *Iy, const float *in, int width, int height);

//Filtro Gaussiano

/*
 
    Kernel:
    [ 1  2  1 ]
    [ 2  4  2 ]  /  16
    [ 1  2  1 ]
  
  param out     Buffer de salida  (width * height floats)
  param in      Buffer de entrada (width * height floats)
  param width   Ancho en píxeles
  param height  Alto en píxeles
   
*/

//Floar -> float
void gaussian_filter(float *out, const float *in, int width, int height);


#endif 




