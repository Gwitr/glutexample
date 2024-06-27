// The fragment shader runs once for every pixel that's drawn. It decides what color and alpha the pixel will have.

#version 330 core

// Receives the UV coordinate from the vertex shader. Note that for fragments that rest between vertices, OpenGL will automatically interpolate the value. The interpolation takes the vertex's
// truncated W component into account, too, something that doesn't matter here, but provides perspective-corrected interpolation in case the vertex shader used a projection matrix
in vec2 uv;

// Writing to this will set the drawn pixel's color
out vec4 outColor;

uniform sampler2D tex;

void main() {
    outColor = texture(tex, uv);  // Every pixel drawn will be sourced from the texture unmodified
}
