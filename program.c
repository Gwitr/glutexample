#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>

#include <GL/glew.h>
#include <GL/gl.h>

#include "program.h"

struct program *used_program = NULL;

struct program *program_new(int nShaders, ...)
{
    // FIXME: Detach shader objects (as of now, they kind of just waste memory until the program is destroyed)
    struct program *program = calloc(1, sizeof *program);
    if (!program)
        goto error;
    program->handle = glCreateProgram();
    if (!program->handle)
        goto error;

    va_list args;
    va_start(args, nShaders);
    for (int i = 0; i < nShaders; ++i) {
        GLint shader = glCreateShader(va_arg(args, GLenum));
        if (!shader)
            goto parseargs_error;
        const char *src = va_arg(args, const char *);
        if (!src)
            goto parseargs_error;
        GLint len = strlen(src);
        glShaderSource(shader, 1, &src, &len);
        glCompileShader(shader);
        GLint success = 0;
        glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
        if (success == GL_FALSE) {
            GLint loglen = 0;
            glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &loglen);
            char *err = malloc(loglen);
            if (!err) {
                fprintf(stderr, "shader compilation error: no diagnostic: out of memory\n");
            } else {
                glGetShaderInfoLog(shader, loglen, &loglen, err);
                fprintf(stderr, "shader compilation error: %.*s\n", loglen, err);
            }
            goto parseargs_error;
        }
        glAttachShader(program->handle, shader);
        glDeleteShader(shader);  // OpenGL will only actually follow through with the deletion once the program is deleted
    }
    va_end(args);

    glLinkProgram(program->handle);
    GLint success = 0;
    glGetProgramiv(program->handle, GL_LINK_STATUS, &success);
    if (success == GL_FALSE) {
        GLint loglen = 0;
        glGetProgramiv(program->handle, GL_INFO_LOG_LENGTH, &loglen);
        char *err = malloc(loglen);
        if (!err) {
            fprintf(stderr, "opengl program linking error: no diagnostic: out of memory\n");
        } else {
            glGetProgramInfoLog(program->handle, loglen, &loglen, err);
            fprintf(stderr, "opengl program linking error: %.*s\n", loglen, err);
        }
        goto error;
    }

    return program;

    parseargs_error:
    va_end(args);
    error:
    if (program) {
        if (program->handle)
            glDeleteProgram(program->handle);
        free(program);
    }
    return NULL;
}

void program_delete(struct program *program)
{
    free(program->uniformDims);
    free(program->uniformNames);
    free(program->uniformTypes);
    free(program->uniformHandles);
    glDeleteProgram(program->handle);
    free(program);
}

static int cmpfunc_strcmp(const void *lhs, const void *rhs)
{
    return strcmp((const char *)lhs, (const char *)rhs);
}

static int get_n_uniform_defs(va_list args)
{
    va_list args2;
    va_copy(args2, args);
    int nUniformDefs = 0;
    for (;;++nUniformDefs) {
        GLenum type = va_arg(args2, GLenum);
        if (type == NO_MORE_UNIFORMS)
            break;
        va_arg(args2, int);
        va_arg(args2, const char *);
    }
    va_end(args2);
    return nUniformDefs;
}

int program_define_uniforms(struct program *program, ...)
{
    if (!program)
        return 1;
    if (program->uniformNames)
        return 1;  // Uniforms are already defined, don't goto error as that'd destroy them

    va_list args;
    va_start(args, program);
    int nUniformDefs = (program->nUniformNames = get_n_uniform_defs(args));

    if (!(program->uniformDims = malloc(nUniformDefs * sizeof(int))) ||
        !(program->uniformNames = malloc(nUniformDefs * (32 + sizeof(int)))) ||
        !(program->uniformTypes = malloc(nUniformDefs * sizeof(GLenum))) ||
        !(program->uniformHandles = malloc(nUniformDefs * sizeof(GLint))))
        goto error;

    for (int i = 0; i < nUniformDefs; ++i) {
        GLenum type = va_arg(args, GLenum);
        int nDims = va_arg(args, int);
        const char *name = va_arg(args, const char *);

        program->uniformHandles[i] = glGetUniformLocation(program->handle, name);
        if (program->uniformHandles[i] == -1) {
            fprintf(stderr, "unknown uniform %s\n", name);
            goto error;
        }
        program->uniformDims[i] = nDims;
        program->uniformTypes[i] = type;

        memset(program->uniformNames + i * (32 + sizeof(int)), 0, 32 + sizeof(int));
        strncpy(program->uniformNames + i * (32 + sizeof(int)), name, 31);
        memcpy(program->uniformNames + i * (32 + sizeof(int)) + 32, &i, sizeof(int));
    }
    va_end(args);

    qsort(program->uniformNames, nUniformDefs, 32 + sizeof(int), cmpfunc_strcmp);
    return 0;

    error:
    free(program->uniformDims);
    program->uniformDims = NULL;
    free(program->uniformNames);
    program->uniformNames = NULL;
    free(program->uniformTypes);
    program->uniformTypes = NULL;
    free(program->uniformHandles);
    program->uniformHandles = NULL;
    va_end(args);
    return 1;
}

void program_use(struct program *program)
{
    if (used_program == program)
        return;
    glUseProgram((used_program = program)->handle);
}

// I miss C++

#define SWITCH_GL_UNIFORM(ts, tn, vtn) switch (n) { \
case 1: glUniform1##ts(handle, (tn)va_arg(args, vtn)); break; \
case 2: ts##Tmp1 = (tn)va_arg(args, vtn); glUniform2##ts(handle, ts##Tmp1, (tn)va_arg(args, vtn)); break; \
case 3: ts##Tmp1 = (tn)va_arg(args, vtn); ts##Tmp2 = (tn)va_arg(args, vtn); glUniform3##ts(handle, ts##Tmp1, ts##Tmp2, (tn)va_arg(args, vtn)); break; \
case 4: ts##Tmp1 = (tn)va_arg(args, vtn); ts##Tmp2 = (tn)va_arg(args, vtn); ts##Tmp3 = (tn)va_arg(args, vtn); glUniform4##ts(handle, ts##Tmp1, ts##Tmp2, ts##Tmp3, (tn)va_arg(args, vtn)); \
} break;

void program_set_uniform(struct program *program, const char *uniformName, ...)
{
    program_use(program);

    char namebuf[32 + sizeof(int)] = { 0 };
    strncpy(namebuf, uniformName, 31);
    void *addr = bsearch(namebuf, program->uniformNames, program->nUniformNames, 32 + sizeof(int), cmpfunc_strcmp);
    if (!addr) {
        fprintf(stderr, "unknown uniform %s\n", uniformName);
        abort();
    }
    unsigned int idx = *(int*)((char*)addr + 32);
    GLint handle = program->uniformHandles[idx];
    GLenum componentType = program->uniformTypes[idx];
    int n = program->uniformDims[idx];

    GLint iTmp1, iTmp2, iTmp3;
    GLfloat fTmp1, fTmp2, fTmp3;
    GLuint uiTmp1, uiTmp2, uiTmp3;
    va_list args;
    va_start(args, uniformName);
    switch (componentType) {
    case GL_UNSIGNED_INT: SWITCH_GL_UNIFORM(ui, GLuint, GLuint)
    case GL_INT:          SWITCH_GL_UNIFORM(i, GLint, GLint)
    case GL_FLOAT:        SWITCH_GL_UNIFORM(f, GLfloat, double)
    }
    va_end(args);
}

#undef SWITCH_GL_UNIFORM
