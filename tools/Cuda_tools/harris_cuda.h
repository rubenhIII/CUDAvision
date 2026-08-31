#ifndef HARRIS_CUDA_H
#define HARRIS_CUDA_H
 
// Non-Maximum Suppression
void non_max_suppression(float* R, int w, int h, unsigned char* out,
                          float threshold, int max_points,
                          int *points_x, int *points_y);

// Harris completo
void harris_detect(unsigned char* gray, int width, int height,
                    float threshold, int max_points,
                    int *points_x, int *points_y);

#endif 