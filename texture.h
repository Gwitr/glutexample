#ifndef TEXTURE_H
#define TEXTURE_H

#include <GL/gl.h>

#include "loadpng.h"

struct texture
{
    GLuint handle;
    int width, height;
};

extern struct texture *bound_texture;

struct texture *texture_new(const char *filename, GLenum minFilter, GLenum magFilter, GLenum sWrap, GLenum tWrap);
void texture_delete(struct texture *texture);

void texture_bind(struct texture *texture);
void texture_set_active(int slot);

#endif  // TEXTURE_H
