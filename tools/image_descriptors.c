#include "image_descriptors.h"
#include <stdlib.h>
#include <math.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

// Genera un número uniforme en (0,1)
double rand_uniform() {
    return (rand() + 1.0) / (RAND_MAX + 2.0);
}

// Genera un número gaussiano N(0,1)
double rand_normal() {
    double u1 = rand_uniform();
    double u2 = rand_uniform();
    
    return sqrt(-2.0 * log(u1)) * cos(2.0 * M_PI  * u2);
}

// Genera coordenada con sigma
int sample_point(double sigma, int half_patch) {
    double val = sigma * rand_normal();
    
    int coord = (int)round(val);
    
    // Clamp al parche
    if (coord > half_patch) coord = half_patch;
    if (coord < -half_patch) coord = -half_patch;
    
    return coord;
}