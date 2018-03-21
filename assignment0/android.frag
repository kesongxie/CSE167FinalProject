#version 330 core
// This is a sample fragment shader.

uniform mat4 modelview;

// Inputs to the fragment shader are the outputs of the same name from the vertex shader.
// Note that you do not have access to the vertex shader's default output, gl_Position.
in vec3 objVpos;
in vec3 objNormal;

// You can output many things. The first vec4 type output determines the color of the fragment
out vec4 FragColor;

void main()
{
    // normal coloring
//    vec3 n = normalize(objNormal)*0.5 + 0.5;
//    FragColor = vec4(n, 1.0f);
    
    vec3 n = normalize(mat3(modelview) * objNormal);
    vec3 p = vec3(modelview * vec4(objVpos.x, objVpos.y, objVpos.z, 1.0));
    vec3 v = normalize(-p);
    float vdn = 1.0 - max(dot(v, n), 0.0);
    
    FragColor.a = 1.0;
    //     float sm = smoothstep(0.0, 1.0, vdn);
    //FragColor.rgb = vec3(1.0f, 0.87f + sm, );
    FragColor.rgb = vec3(smoothstep(0.0, 1.0, vdn));
}
