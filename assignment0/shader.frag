#version 330 core
// This is a sample fragment shader.

// Inputs to the fragment shader are the outputs of the same name from the vertex shader.
// Note that you do not have access to the vertex shader's default output, gl_Position.
in float sampleExtraOutput;

in vec4 inColor;

uniform sampler2D texture1;
uniform mat4 projection;
uniform mat4 modelview;

out vec4 FragColor;
in vec2 texCoord;
in vec4 vSpace;

// rim shading
in vec3 objVpos;
in vec3 objNormal;

// fog param
vec3 fogColor = vec3(0, 0, 0);
float FogDensity = 0.8;
float dist = 0;
float fogFactor = 0;



void main()
{
    // Color everything a hot pink color. An alpha of 1.0f means it is not transparent.
//    color = vec4(1.0f, 0.41f, 0.7f, sampleExtraOutput);
//    color = texture(texture1, texCoord);
//    FragColor = inColor;
    // linear fog
    dist = abs(vSpace.z);
    fogFactor = (1000 - dist)/1000;
    fogFactor = clamp( fogFactor, 0.0, 1.0 );
    FragColor = mix(vec4(fogColor,1), inColor, fogFactor);
}
