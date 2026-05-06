#include "image_descriptors.h"
#include <stdlib.h>
#include <math.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

// Genera un número uniforme en (0,1)
double uniform()
{
    return (rand() + 1.0) / (RAND_MAX + 2.0);
}

// Genera un número gaussiano N(0,1)
double normal()
{
    double u1 = uniform();
    double u2 = uniform();

    double z0 = sqrt(-2.0 * log(u1)) * cos(2 * M_PI * u2);
    double z1 = sqrt(-2.0 * log(u1)) * cos(2 * M_PI * u2);

    return z0;
}

double rnorm(double mu, double sigma)
{
    return mu + sigma * normal();
}

// Genera coordenada con sigma
int sample_point(double sigma, int center_coord, int patch_size)
{
    double val = sigma * normal();
    int half_patch = (int) patch_size / 2;

    int coord = (int)round(val);
    
    // Clamp al parche
    if (coord > half_patch) coord = half_patch;
    if (coord < -half_patch) coord = -half_patch;

    coord = center_coord + coord;
    
    return coord;
}

// Prueba binaria entre coordenadas dentro de un parche
int binary_test(unsigned char *gray_image, int width, int height, int x0, int y0, int x1, int y1)
{
    int idx0 = (y0 * width + x0) * 3;
    int idx1 = (y1 * width + x1) * 3;

    int result = gray_image[idx0] < gray_image[idx1] ? 1 : 0;
    return result;
}