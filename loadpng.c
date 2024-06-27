#include <png.h>
#include <string.h>
#include <stdlib.h>

void *loadpng(const char *filename, int *outwidth, int *outheight, int *outpitch)
{
    // I won't be documenting this code sorry
    png_structp png_ptr = NULL;
    png_infop info_ptr = NULL, end_info = NULL;

    FILE *fp = fopen(filename, "rb");
    if (!fp)
        goto error;
    png_byte header[8];
    fread(header, 1, 8, fp);
    if (png_sig_cmp(header, 0, 8))
        goto error;

    png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    if (!png_ptr)
        goto error;

    if (!(info_ptr = png_create_info_struct(png_ptr)))
        goto error;

    if (!(end_info = png_create_info_struct(png_ptr)))
        goto error;

    if (setjmp(png_jmpbuf(png_ptr)))
        goto error;

    png_init_io(png_ptr, fp);
    png_set_sig_bytes(png_ptr, 8);

    png_read_png(png_ptr, info_ptr, PNG_TRANSFORM_BGR | PNG_TRANSFORM_PACKING | PNG_TRANSFORM_STRIP_16 | PNG_TRANSFORM_EXPAND, NULL);

    png_bytep *row_pointers = png_get_rows(png_ptr, info_ptr);
    int width = png_get_image_width(png_ptr, info_ptr), height = png_get_image_height(png_ptr, info_ptr), has_alpha = png_get_channels(png_ptr, info_ptr) == 4;

    int pitch = 3 + has_alpha;
    void *pixels = malloc(width * height * pitch);
    if (!pixels)
        abort();
    for (int i = 0; i < height; ++i)
        memcpy((char*)pixels + i * width * pitch, row_pointers[i], width * pitch);

    png_destroy_read_struct(&png_ptr, &info_ptr, &end_info);
    fclose(fp);
    if (outwidth)
        *outwidth = width;
    if (outheight)
        *outheight = height;
    if (outpitch)
        *outpitch = pitch;
    return pixels;

    error:
    if (png_ptr) {
        if (info_ptr)
            if (end_info)
                png_destroy_read_struct(&png_ptr, &info_ptr, &end_info);
            else
                png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
        else
            png_destroy_read_struct(&png_ptr, NULL, NULL);
    }
    if (fp)
        fclose(fp);
    return NULL;
}
