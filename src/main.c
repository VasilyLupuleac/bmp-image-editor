#include "bmp.h"
#include <stdlib.h>
#include <stdio.h>

int main(int argv, char** argc)
{
    if (argv != 8)
        return 1;

    char* fin = argc[2];
    char* fout = argc[3];

    int x = atoi(argc[4]);
    int y = atoi(argc[5]);
    int width = atoi(argc[6]);
    int height = atoi(argc[7]);

    if (width == 0 || height == 0)
    {
        return 1;
    }

    image_t* image = load_bmp(fin);

    if (!image)
    {
        return 1;
    }

    image_t* cropped_image = crop(image, x, y, width, height);

    if (!cropped_image)
    {
        return 1;
    }
    image_t* rotated_image = rotate(cropped_image);

    if (!rotated_image)
    {
        return 1;
    }

    if (save_bmp(rotated_image, fout))
    {
        return 1;
    }

    clear_image(image);
    clear_image(cropped_image);
    clear_image(rotated_image);

    return 0;
}
