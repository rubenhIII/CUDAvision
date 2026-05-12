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
int sample_point(double sigma, int center_coord, int patch_size, int limit)
{
    double val = sigma * normal();
    int half_patch = (int) patch_size / 2;

    int coord = (int)round(val);
    
    // Clamp al parche
    if (coord > half_patch) coord = half_patch;
    if (coord < -half_patch) coord = -half_patch;

    coord = center_coord + coord;
    coord = coord < 0 ? 0 : coord % limit;
    
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

void brief_descriptor(unsigned char * gray, int width, int height, unsigned char **descriptors, int descriptor_len, int *points_x, int *points_y, int max_points)
{
    for (int p = 0; p < max_points; p++) {
        int x = points_x[p];
        int y =  points_y[p];

        //draw_point(gray, width, height, x, y, GREEN);
        //draw_rectangle(gray, width, height, 400, 400, 500, 500);
        for (int d = 0; d < descriptor_len; d++) {
            int x0 = sample_point(20, x, 100, width);
            int y0 = sample_point(20, y, 100, height);

            int x1 = sample_point(20, x, 100, width);
            int y1 = sample_point(20, y, 100, height);
            
            //printf("x: %d y: %d x0: %d y0: %d x1: %d y1: %d", x, y, x0, y0, x1, y1);
            descriptors[d][p] = binary_test(gray, width, height, x0, y0, x1, y1);

            //draw_point(gray, width, height, x0, y0, RED);
            //draw_point(gray, width, height, x1, y1, BLUE);

            //printf("%d ", descriptors[d][p]);
        }
        //printf("\n");
    }

}