#ifndef _BoundingBox_H_
#define _BoundingBox_H_

#ifdef __APPLE__
#include <OpenGL/gl3.h>
#include <OpenGL/glext.h>
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

class BoundingBox
{
public:
    BoundingBox();
    ~BoundingBox();
    
    glm::mat4 bbox_toWorld;
    
    GLuint bboxShaderProgram;
    float max_x, max_y, max_z, min_x, min_y, min_z;
    bool collide; // collide flag used in fragment shader
    
    void draw(GLuint, glm::mat4);
    void setBoundaries(float, float, float, float, float, float);
    
    // These variables are needed for the shader program
    GLuint bbox_VBO, bbox_VAO, bbox_EBO;
    GLuint bbox_uProjection, bbox_uModelview;
};

#endif


