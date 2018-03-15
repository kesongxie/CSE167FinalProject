#version 330 core
// This is a sample fragment shader.

// Inputs to the fragment shader are the outputs of the same name from the vertex shader.
// Note that you do not have access to the vertex shader's default output, gl_Position.
in vec3 objNormal;

// You can output many things. The first vec4 type output determines the color of the fragment
out vec4 FragColor;

void main()
{
    // normal coloring
    vec3 n = normalize(objNormal)*0.5 + 0.5;
    FragColor = vec4(n, 1.0f);
}
