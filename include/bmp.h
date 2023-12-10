#pragma once

#include <stdint.h>

#pragma pack(push, 1)

typedef struct tagBITMAPHEADER
{
    uint16_t bfType;
    uint32_t bfSize;
    uint16_t bfReserved1;
    uint16_t bfReserved2;
    uint32_t bfOffBits;

    uint32_t biSize;
    int32_t  biWidth;
    int32_t  biHeight;
    uint16_t  biPlanes;
    uint16_t  biBitCount;
    uint32_t biCompression;
    int32_t biSizeImage;
    uint32_t  biXPelsPerMeter;
    uint32_t  biYPelsPerMeter;
    uint32_t biClrUsed;
    uint32_t biClrImportant;
} BITMAPHEADER;

typedef struct pixel_s
{
    char b;
    char g;
    char r;
} pixel_t;

#pragma pack(pop)

typedef struct image_s
{
    pixel_t** pixels;
    BITMAPHEADER* header;
    int padding;
} image_t;

int save_bmp(image_t* image, char* filename);
image_t* load_bmp(char* filename);
image_t* crop(image_t* image, int x, int y, int new_width, int new_height);
image_t* rotate(image_t* image);
void clear_image(image_t* image);

