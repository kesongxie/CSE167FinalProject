#version 330 core
// This is a sample fragment shader.

// Inputs to the fragment shader are the outputs of the same name from the vertex shader.
// Note that you do not have access to the vertex shader's default output, gl_Position.

// You can output many things. The first vec4 type output determines the color of the fragment
out vec4 FragColor;

uniform int collide;

void main()
{
    if (collide == 0) {
        // no collision: white
        FragColor = vec4(1.0f, 1.0f, 1.0f, 1.0f);
    } else {
        // collision: red
        FragColor = vec4(1.0f, 0.0f, 0.0f, 1.0f);
    }
}
