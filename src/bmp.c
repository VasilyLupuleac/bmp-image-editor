#include "bmp.h"
#include <stdlib.h>
#include <stddef.h>
#include <string.h>
#include <stdio.h>

void assign_values(BITMAPHEADER* header, int32_t* width, int32_t* height)
{
    *width = header->biWidth;
    *height = header->biHeight;
}

void set_size(BITMAPHEADER* header, int32_t width, int32_t height)
{
    header->biWidth = width;
    header->biHeight = height;
}

BITMAPHEADER* copy_header(BITMAPHEADER* header)
{
    BITMAPHEADER* new_header = (BITMAPHEADER*)malloc(sizeof(BITMAPHEADER));
    if (header != NULL)
    {
        *new_header = *header;
    }
    return new_header;
}

pixel_t** make_pixels(int32_t width, int32_t height)
{
    pixel_t** pixels = (pixel_t**)malloc(sizeof(pixel_t*) * height);
    pixel_t* data = (pixel_t*)malloc(sizeof(pixel_t) * width * height);
    for (int32_t i = 0; i < height; i++)
    {
        pixels[i] = data + i * width;
    }
    return pixels;
}

image_t* new_image(BITMAPHEADER* header)
{
    int32_t width, height;
    assign_values(header, &width, &height);

    image_t* image = (image_t*)malloc(sizeof(image_t));

    image->padding = width % 4;
    image->pixels = make_pixels(width, height);
    image->header = header;

    int32_t full_width = sizeof(pixel_t) * width + sizeof(char) * (image->padding);
    header->bfSize = full_width * height + sizeof(BITMAPHEADER);

    return image;
}

void clear_image(image_t* image)
{
    free(image->header);
    free(image->pixels[0]);
    free(image->pixels);
    free(image);
}

image_t* load_bmp(char* filename)
{
    FILE* file = fopen(filename, "r");
    if (file == NULL)
    {
        return NULL;
    }

    BITMAPHEADER* header = copy_header(NULL);
    fread(header, sizeof(BITMAPHEADER), 1, file);

    image_t* image = new_image(header);

    int32_t width, height;
    assign_values(header, &width, &height);

    pixel_t** pixels = image->pixels;
    int padding = image->padding;

    for (int32_t i = 0; i < height; i++)
    {
        fread(pixels[i], sizeof(pixel_t) * width, 1, file);
        fseek(file, padding, SEEK_CUR);
    }
    
    fclose(file);
    return image;
}

int save_bmp(image_t* image, char* filename)
{
    FILE* file = fopen(filename, "w");
    if (file == NULL)
    {
        return 1;
    }

	BITMAPHEADER* header = image->header;
    int32_t width, height;
    assign_values(header, &width, &height);

    pixel_t** pixels = image->pixels;

    int padding = image->padding;
    char extra_bytes[padding];
    memset(extra_bytes, 0, padding);

    fwrite(header, sizeof(BITMAPHEADER), 1, file);
    for (int32_t i = 0; i < height; i++)
    {
        fwrite(pixels[i], sizeof(pixel_t) * width, 1, file);
        fwrite(extra_bytes, sizeof(char) * padding, 1, file);
    }

    fclose(file);
	return 0;
}

image_t* crop(image_t* image, int32_t x, int32_t y, int32_t new_width, int32_t new_height)
{
    if (x < 0 || y < 0)
    {
        return NULL;
    }
    if (new_width < 0 || new_height < 0)
    {
        return NULL;
    }

    BITMAPHEADER* old_header = image->header;
    int32_t old_width, old_height;
    assign_values(old_header, &old_width, &old_height);
    if (x + new_width > old_width || y + new_height > old_height)
    {
        return NULL;
    }

	BITMAPHEADER* new_header = copy_header(old_header);
	set_size(new_header,new_width, new_height);
	image_t* cropped_image = new_image(new_header);

    pixel_t** old_pixels = image->pixels;
    pixel_t** new_pixels = cropped_image->pixels;

    y = old_height - new_height - y;
    for (int i = 0; i < new_height; i++)
    {
        memcpy(new_pixels[i], old_pixels[i + y] + x, sizeof(pixel_t) * new_width);
    }

    return cropped_image;
}

image_t* rotate(image_t* image)
{
	BITMAPHEADER* old_header = image->header;
    BITMAPHEADER* new_header = copy_header(old_header);

    int32_t new_width, new_height;
    assign_values(old_header, &new_height, &new_width);
    set_size(new_header,new_width, new_height);
    image_t* rotated_image = new_image(new_header);

    pixel_t** old_pixels = image->pixels;
    pixel_t** new_pixels = rotated_image->pixels;

    for (int32_t i = 0; i < new_height; i++)
    {
        for (int32_t j = 0; j < new_width; j++)
        {
           new_pixels[i][j] = old_pixels[j][new_height - i - 1];
        }
    }

    new_header->biXPelsPerMeter = old_header->biYPelsPerMeter;
    new_header->biYPelsPerMeter = old_header->biXPelsPerMeter;

    return rotated_image;
}
