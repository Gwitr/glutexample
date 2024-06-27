#include <GL/glew.h>
#include <GL/gl.h>

GLuint vboHandle, vaoHandle;

const GLfloat vertexData[] = {
/*  position      uv          */
    -1.0f, -1.0f,  0.0f, 1.0f,
    -1.0f, +1.0f,  0.0f, 0.0f,
    +1.0f, -1.0f,  1.0f, 1.0f,
    +1.0f, -1.0f,  1.0f, 1.0f,
    +1.0f, +1.0f,  1.0f, 0.0f,
    -1.0f, +1.0f,  0.0f, 0.0f
};
const GLint nVertices = sizeof(vertexData) / sizeof(GLfloat);

void quad_init(void)
{
    // Create a VBO
    glGenBuffers(1, &vboHandle);
    // Bind it to the GL_ARRAY_BUFFER target
    glBindBuffer(GL_ARRAY_BUFFER, vboHandle);
    // Upload the blob stored in `vertexData` to whatever's bound to the GL_ARRAY_BUFFER target, with memory access hint GL_STATIC_DRAW (we won't be modifying it much, and we'll be using it to draw)
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertexData), vertexData, GL_STATIC_DRAW);

    // Create a VAO
    glGenVertexArrays(1, &vaoHandle);
    // Bind it; unlike VBOs (that have various targets to bind to), only 1 VAO may be bound at a time
    glBindVertexArray(vaoHandle);
    // Bind the VBO again so that the VAO remembers the glBindBuffer call (i don't know if this is necessary??)
    glBindBuffer(GL_ARRAY_BUFFER, vboHandle);
    // Define vertex attribute 0 as a 2D vector of floats, and specify that, in a VBO, it is packed with 2 `GLfloat`s worth of padding and the first one occurs at byte offset 0. This vertex attribute will serve as the vertex position in 2D space.
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 4, (void*)0);
    glEnableVertexAttribArray(0);
    // Define vertex attribute 1 as a 2D vector of floats, and specify that, in a VBO, it is packed with 2 `GLfloat`s worth of padding and the first one occurs at byte offset 2*sizeof(GLfloat). This vertex attribute will serve as the vertex UV coordinate.
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 4, (void*)(sizeof(GLfloat) * 2));
    glEnableVertexAttribArray(1);

    glBindVertexArray(0);
}

void quad_deinit(void)
{
    glDeleteVertexArrays(1, &vaoHandle);
    glDeleteBuffers(1, &vboHandle);
}

void quad_draw(void)
{
    // Bind the VAO; this reloads all the settings we've saved in it
    glBindVertexArray(vaoHandle);
    // Draw triangles from the currently bound GL_ARRAY_BUFFER, vertices [0; nVertices)
    glDrawArrays(GL_TRIANGLES, 0, nVertices);
}
