#ifndef IMAGE_DESCRIPTORS_H
#define IMAGE_DESCRIPTORS_H

double uniform();
double normal();
double rnorm(double mu, double sigma);
int sample_point(double sigma, int center_coord, int patch_size);

#endif