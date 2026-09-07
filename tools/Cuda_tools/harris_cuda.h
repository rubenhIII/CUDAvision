#ifndef HARRIS_CUDA_H
#define HARRIS_CUDA_H
// Harris completo
int harris_detect(
    unsigned char* gray, 
    int width, 
    int height, 
    float threshold, 
    int max_points, 
    int *points_x, 
    int *points_y, 
    unsigned char* imagen_gris_brief);

#endif

