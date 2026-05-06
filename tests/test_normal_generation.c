#include <stdlib.h>
#include <stdio.h>
#include "../tools/image_descriptors.h"

char ans = ' ';

int main()
{

    printf("Enter an option to test: \n");
    printf("1. Generate from uniform\n");
    printf("2. Generate from normal\n");
    printf("3. Generate coordinates\n");
    while (ans != '4')
    {
        scanf(" %c", &ans);
        switch (ans)
        {
            case '1':
                printf("Sample from uniform: %f\n", uniform());
                break;
            case '2':
                printf("Sample from normal: %f\n", normal());
                break;
            case '3':
                printf("Coordinates: %d\n", sample_point(2, 10, 20));    
                break;
            case '4':
                return 0;
            default:
                printf("Opcion no valida %c\n", ans);
        }
    }
    

    return 0;
}
