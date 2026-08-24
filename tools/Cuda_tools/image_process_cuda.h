#ifndef IMAGE_PROCESS_CUDA_H
#define IMAGE_PROCESS_CUDA_H



// Los filtros reciben una imagen en escala de grises
// almacenada como float.


/* Filtro Gaussiano
  salida    -> imagen de salida
  entrada     -> imagen de entrada
  ancho  -> ancho de la imagen
  altura -> alto de la imagen
*/


void gaussian_filter_cuda(
    float *salida,
    const float *entrada,
    int ancho,
    int altura
);



// Filtro Sobel

// Gx -> Sobel horizontal (GX)
// Gy -> Sobel vertical (GY)
// imagen -> imagen de entrada



void sobel_filter_cuda(
    float *Ix,
    float *Iy,
    const float *in,
    int width,
    int height
);

#endif