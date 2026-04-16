#include <stdlib.h>
#include <stdio.h>
#include "grayscale.h"

unsigned char r[27] = {
    1, 1 ,1, 
    2, 2, 2,
    3, 3, 3,
    4, 4, 4, 
    5, 5, 5, 
    6, 6, 6, 
    7, 7, 7,
    8, 8, 8,
    9, 9, 9
};
unsigned char **gray;
int channel_len = 27;
int w = 3, h = 3;

int main()
{
    //gray = image_rgb_channel(r, channel_len, w, h, 'r');
    //if (gray) image_print(gray, w, h);
    //else printf("There was an error in conversion");

    int width, height, channels;
    unsigned char *lena, *lena_gray;
    lena = image_load("lena.jpeg", &width, &height, &channels);
    lena_gray = image_to_gray_vector(lena, width, height);
    image_show(lena_gray, width, height);

    free(lena);
    free(lena_gray);

    return 0;
}