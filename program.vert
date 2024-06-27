// The vertex shader runs once for every vertex that's drawn. It transforms a datum (probably taken from a VBO) to produce a 4D vector (that it assigns to gl_Position).
// The vector is then transformed by OpenGL into a vector representing a screen position (by dividing the vector by its own W component, and then truncating the last 2
// components, turning it into a 2D vector. This means that the Z coordinate is ignored by OpenGL (except for clipping purposes) - it is up to the vertex shader to do
// perspective projection). Both components of the final 2D vector are expected to range from -1 to 1

#version 330 core

layout(location=0) in vec2 inVertexPosition;  // Maps to vertex attribute 0
layout(location=1) in vec2 inVertexUV;        // Maps to vertex attribute 1

// This variable will pass the UV coordinate gathered from the vertex data forward to the fragment shader.
out vec2 uv;

// This variable is assigned to by the CPU, and refers to the offset of what we'll render
uniform vec2 position;

void main() {
    uv = inVertexUV;  // Pass the input vertex UV forward to the fragment shader
    gl_Position = vec4(position + inVertexPosition, 0.0f, 1.0f);
}
