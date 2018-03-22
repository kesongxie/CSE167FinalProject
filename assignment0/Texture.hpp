//
//  Texture.hpp
//  assignment0
//
//  Created by Xie kesong on 3/16/18.
//  Copyright © 2018 ___CSE167___. All rights reserved.
//


#ifndef TEXTURE_HPP
#define TEXTURE_HPP
#define GLFW_INCLUDE_GLEXT
#ifdef __APPLE__
#define GLFW_INCLUDE_GLCOREARB
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
#include <vector>


// Load a .BMP file using our custom loader
GLuint loadBMP_custom(const char * imagepath);

// Load a .DDS file using GLFW's own loader
GLuint loadDDS(const char * imagepath);

#endif
