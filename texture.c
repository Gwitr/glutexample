#include <stdlib.h>

#include <GL/glew.h>
#include <GL/gl.h>

#include "loadpng.h"
#include "texture.h"

struct texture *bound_texture = NULL;

struct texture *texture_new(const char *filename, GLenum minFilter, GLenum magFilter, GLenum sWrap, GLenum tWrap)
{
    int width, height, pitch;
    void *pixels = loadpng(filename, &width, &height, &pitch);
    if (!pixels)
        return NULL;

    struct texture *texture = calloc(1, sizeof *texture);
    if (!texture) {
        free(pixels);
        return NULL;
    }
    texture->width = width;
    texture->height = height;

    GLuint tex;
    glGenTextures(1, &tex);
    texture->handle = tex;
    struct texture *prev_bound_texture = bound_texture;
    texture_bind(texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, sWrap);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, tWrap);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, minFilter);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, magFilter);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, pitch == 4 ? GL_BGRA : GL_BGR, GL_UNSIGNED_BYTE, pixels);
    free(pixels);
    glGenerateMipmap(GL_TEXTURE_2D);

    if (prev_bound_texture)
        texture_bind(prev_bound_texture);

    return texture;
}

void texture_delete(struct texture *texture)
{
    glDeleteTextures(1, &texture->handle);
    free(texture);
}

void texture_bind(struct texture *texture)
{
    if (texture == bound_texture)
        return;
    glBindTexture(GL_TEXTURE_2D, (bound_texture = texture)->handle);
}

void texture_set_active(int slot)
{
    glActiveTexture(GL_TEXTURE0 + slot);
}
