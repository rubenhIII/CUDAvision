#ifndef HARRIS_H
#define HARRIS_H

void harris_detect(unsigned char* gray, int width, int height, float threshold, int max_points, int *points_x, int *points_y);

#endif