#ifndef IMAGE_DESCRIPTORS_H
#define IMAGE_DESCRIPTORS_H

double uniform();
double normal();
double rnorm(double mu, double sigma);
int sample_point(double sigma, int center_coord, int patch_size);
int binary_test(unsigned char *gray_image, int width, int height, int x0, int y0, int x1, int y1);

#endif