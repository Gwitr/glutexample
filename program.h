#ifndef PROGRAM_H
#define PROGRAM_H

#include <GL/gl.h>

#define NO_MORE_UNIFORMS (1ULL<<31)

// Holds information about an OpenGL program
struct program
{
    GLint handle;
    char *uniformNames;
    GLint *uniformHandles;
    GLenum *uniformTypes;
    int *uniformDims;
    int nUniformNames;
};

extern struct program *used_program;

// Creates a new program, compiling and linking a list of shaders (in the form of GLenum and const char * pairs)
struct program *program_new(int nShaders, ...);
// Deletes a program
void program_delete(struct program *program);

// Defines the uniforms present inside of a program. Every uniform is represented by 3 parameters - a type (GLenum),
// the number of vector components (int), and a name (const char *). The last vararg is expected to be NO_MORE_UNIFORMS.
int program_define_uniforms(struct program *program, ...);
// Assigns a program's uniform by name. The types and number of varargs depends on how the uniform was defined (see above).
void program_set_uniform(struct program *program, const char *uniformName, ...);

// Sets the currently used program, allowing for it to be used to draw stuff
void program_use(struct program *program);

#endif  // PROGRAM_H
