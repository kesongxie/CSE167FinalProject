//
//  Skybox.hpp
//  assignment0
//
//  Created by Xie kesong on 3/13/18.
//  Copyright © 2018 ___CSE167___. All rights reserved.
//

#ifndef Skybox_hpp
#define Skybox_hpp
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
#include "shader.h"
#include <string>
#include <vector>


class Skybox {
public:
    Skybox(GLint program);
    void render();
private:
    GLint skyboxshaderProgram;
    GLuint skyboxVAO, skyboxVBO;
    unsigned int cubemapTexture;
    unsigned int loadCubemap(std::vector<std::string> faces);
};

#endif /* Skybox_hpp */
