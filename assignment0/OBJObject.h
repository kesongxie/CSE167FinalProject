#ifndef OBJOBJECT_H
#define OBJOBJECT_H

#ifdef __APPLE__
#include <OpenGL/gl3.h>
#include <OpenGL/glext.h>
#else
#include <GL/glew.h>
#endif

#include <GLFW/glfw3.h>
#include <glm/mat4x4.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtc/matrix_transform.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/rotate_vector.hpp>
#include <vector>
#include <iostream>
#include <cstdio>
#include <limits>
#include <algorithm>
#include "BoundingBox.h"

class OBJObject
{
private:
    
    /* object's coordinates in window when initialized */
    float init_x;
    float init_y;
    float init_z;
    /* object's current coordinates in window */
    float x_coord;
    float y_coord;
    float z_coord;
    
public:
    std::vector<unsigned int> indices;
    std::vector<glm::vec3> vertices;
    std::vector<glm::vec3> rot_vertices;
    std::vector<glm::vec3> dragon_vertices;
    std::vector<glm::vec3> normals;
    glm::mat4 toWorld;
    glm::mat4 toWorld_noRot;
    
    float magnitude; // for normalizing
    float max_dimension; // for scaling all objects into a 2x2x2 cube
    float angle; // currently rotated degrees
    
    // PA4: variables for BoundingBox
    float init_max_x, init_max_y, init_max_z, init_min_x, init_min_y, init_min_z;
    float max_x, max_y, max_z, min_x, min_y, min_z;
    BoundingBox * box;
    glm::vec3 size;
    glm::vec3 center;
    glm::mat4 transform;
    
    // These variables are needed for the shader program
    GLuint VBO, VAO, NBO, EBO;
    GLuint uProjection, uModelview;
    
    OBJObject(const char* filepath);
    ~OBJObject();
    
    void parse(const char* filepath);
    void draw(GLuint);
    
    void followCursor(float, float);
    void move_x(float);
    void move_y(float);
    void move_z(float);
    void spin(float);
};

#endif
