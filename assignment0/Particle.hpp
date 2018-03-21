//
//  Particle.hpp
//  assignment0
//
//  Created by Xie kesong on 3/16/18.
//  Copyright © 2018 ___CSE167___. All rights reserved.
//

#ifndef Particle_hpp
#define Particle_hpp
#define GLFW_INCLUDE_GLEXT
#ifdef __APPLE__
#define GLFW_INCLUDE_GLCOREARB
#include <OpenGL/gl3.h>
#include <OpenGL/glext.h>
#include <OpenGL/gl.h> // Remove this line in future projects

#else
#include <GL/glew.h>
#endif
#include <GLFW/glfw3.h>
// Use of degrees is deprecated. Use radians instead.
#ifndef GLM_FORCE_RADIANS
#define GLM_FORCE_RADIANS
#endif
#include <glm/mat4x4.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <string>
#include <vector>


class Particle {
public:
    glm::vec3 position;
    glm::vec3 velocity;
    float gravityEffect;
    float scale, rotation;
    float lifeLength; // Remaining life of the particle. if < 0 : dead and unused.
    //    unsigned char r,g,b,a; // Color
    float elapsedTime;
    Particle();
    void update();
    int ParticlesCount;
    void render();
  
};

static const GLfloat g_vertex_buffer_data[] = {
    -0.5f, -0.5f, 0.0f,
    0.5f, -0.5f, 0.0f,
    -0.5f, 0.5f, 0.0f,
    0.5f, 0.5f, 0.0f,
};



const int MaxParticles = 100000;
//Particle ParticlesContainer[MaxParticles];

#endif /* Particle_hpp */
