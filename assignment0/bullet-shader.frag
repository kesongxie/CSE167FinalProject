#version 330 core
// This is a sample fragment shader.

// Inputs to the fragment shader are the outputs of the same name from the vertex shader.
// Note that you do not have access to the vertex shader's default output, gl_Position.
in float sampleExtraOutput;

in vec4 inColor;

// You can output many things. The first vec4 type output determines the color of the fragment
out vec4 color;
in vec2 texCoord;
uniform sampler2D texture1;


void main()
{
    // Color everything a hot pink color. An alpha of 1.0f means it is not transparent.
//    color = vec4(1.0f, 0.41f, 0.7f, sampleExtraOutput);
//    color = texture(texture1, texCoord);
    color = inColor;
}
