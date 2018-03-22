#version 330 core
// NOTE: Do NOT use any version older than 330! Bad things will happen!

// This is an example vertex shader. GLSL is very similar to C.
// You can define extra functions if needed, and the main() function is
// called when the vertex shader gets run.
// The vertex shader gets called once per vertex.

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normals;
layout (location = 2) in vec2 v_texCoord;


// Uniform variables can be updated by fetching their location and passing values to that location
uniform mat4 projection;
uniform mat4 modelview;

// Outputs of the vertex shader are the inputs of the same name of the fragment shader.
// The default output, gl_Position, should be assigned something. You can define as many
// extra outputs as you need.
out float sampleExtraOutput;
out vec4 inColor;
out vec2 texCoord;

//rim shading
out vec3 objVpos;
out vec3 objNormal;

// fog param
out vec4 vSpace;


void main()
{
    // OpenGL maintains the D matrix so you only need to multiply by P, V (aka C inverse), and M
    gl_Position = projection * modelview * vec4(position.x, position.y, position.z, 1.0);
    sampleExtraOutput = 1.0f;
    vSpace = modelview * vec4(position,1);
    objVpos = position;
    objNormal = normals;
    inColor = vec4(20.0f / 255.0, 96.0 / 255.0f, 172.0 / 255.0f, 1.0);
//    inColor = vec4(normals.x, normals.y, normals.z, 1.0);
//    texCoord = v_texCoord;
}
