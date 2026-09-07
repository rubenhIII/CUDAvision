#ifndef IMAGE_DESCRIPTORS_CUDA_H
#define IMAGE_DESCRIPTORS_CUDA_H


// Números aleatorios (host, secuenciales)
double uniform();
double normal();
double rnorm(double mu, double sigma);

// Generar patrón de muestreo BRIEF (host, secuencial, se hace una sola vez)
int sample_pattern(double sigma, double **norm_x, double **norm_y, int descriptor_len);

// Utilidades usadas también dentro de los kernels CUDA
int sample_point(double val, int center_coord, int patch_size, int limit);
int binary_test(unsigned char *gray_image, int width, int height, int x0, int y0, int x1, int y1);

// Descriptor BRIEF (CUDA: un hilo por cada bit de cada punto)
void brief_descriptor(unsigned char * gray, int width, int height,
                       unsigned char **descriptors, int descriptor_len,
                       int *points_x, int *points_y, int max_points,
                       double **norm_x, double **norm_y);

// Matching por distancia de Hamming (CUDA: un hilo por punto de la primera imagen)
unsigned int** hamming(unsigned char **descriptors, unsigned char **descriptors2,
                        int descriptor_len, int max_points);



#endif 