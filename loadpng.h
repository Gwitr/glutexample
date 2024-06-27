#ifndef LOADPNG_H
#define LOADPNG_H

// Loads a PNG file, returning a tightly packed 2D array of pixels, of either a BGR24 or a BGRA32 format
// Writes the image size to outwidth and outheight (if not NULL). Writes the length of a pixel, in bytes, to outpitch (if not NULL)
void *loadpng(const char *filename, int *outwidth, int *outheight, int *outpitch);

#endif  // LOADPNG_H
