#ifndef IMAGE_TOOLS_H
#define IMAGE_TOOLS_H

#define BIT_DEPTH 256

typedef enum {
    RED,
    GREEN,
    BLUE,
    COLOR_COUNT
} Color;

unsigned char** image_zeros(int width, int height);
unsigned char ** matrix_zeros(int rows, int columns);
double ** matrix_zeros_d(int rows, int columns);
unsigned char** image_copy(unsigned char **image, int width, int height);
void image_free(unsigned char **image, int width, int height);
void matrix_free_d(double **matrix, int rows, int columns);
void image_print(unsigned char **image, int width, int height);
unsigned char * vec_zeros(int len);
unsigned char** image_to_gray(unsigned char **r, unsigned char **g, unsigned char **b, int width, int height);
unsigned char* image_to_gray_vector(unsigned char *image, int width, int height);
unsigned char** image_rgb_channel(unsigned char *rgb_vector, int vec_size, int width, int height, const char channel);
unsigned char* image_rgb_to_vector(unsigned char **r, unsigned char **g, unsigned char **b, int width, int height);
unsigned char* image_load(const char* filename, int *width, int *height, int *channels);
void image_show(unsigned char *rgb_vector, int width, int height);
unsigned int* image_hist(unsigned char *gray_image, int width, int height);
float* image_hist_norm(unsigned char *gray_image, int width, int height);
float hist_cdf(float *hist_norm, unsigned int n, unsigned int m);
float hist_mean(float *hist_norm, unsigned int n, unsigned int m);
unsigned int otsu_thresholding(float *hist_norm);
void image_threshold(unsigned int th, unsigned char*gray_image, int width, int height);
void draw_point(unsigned char *gray_image, int width, int height, int x, int y, Color color);
void draw_line(unsigned char *gray_image, int width, int height, int x0, int y0, int x1, int y1);
void draw_rectangle(unsigned char *gray_image, int width, int height, int x0, int y0, int x1, int y1);
void image_hstack(unsigned char *image_0, unsigned char *image_1, unsigned char * image_out, int width, int height);


#endif