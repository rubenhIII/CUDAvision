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
        scanf("%c", &ans);
        switch (ans)
        {
        case '1':
            printf("Sample from uniform: %d\n", rand_uniform());
            break;
        case '2':
            printf("Sample from normal: %d\n", rand_normal());
            break;
        case '3':
            printf("Coordinates: %d\n", rand_normal());    
            break;
        default:
            return 0;
        }
    }
    

    return 0;
}
