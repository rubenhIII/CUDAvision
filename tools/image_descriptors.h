#ifndef IMAGE_DESCRIPTORS_H
#define IMAGE_DESCRIPTORS_H

double uniform();
double normal();
double rnorm(double mu, double sigma);
int sample_pattern(double sigma, double **norm_x, double **norm_y, int descriptor_len);
int sample_point(double sigma, int center_coord, int patch_size, int limit);
int binary_test(unsigned char *gray_image, int width, int height, int x0, int y0, int x1, int y1);
void brief_descriptor(unsigned char * gray, int width, int height, unsigned char **descriptors, int descriptor_len, int *points_x, int *points_y, int max_points, double **norm_x, double **norm_y);
unsigned int** hamming(unsigned char **descriptors, unsigned char **descriptors2, int descriptor_len, int max_points);

#endif