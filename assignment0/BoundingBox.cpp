#include "BoundingBox.h"
#include "Window.h"

#define BBOX_VERTEX_SHADER_PATH "./bbox.vert"
#define BBOX_FRAGMENT_SHADER_PATH "./bbox.frag"

GLfloat bbox_vertices[] = {
    -0.5, -0.5, -0.5, 1.0,
    0.5, -0.5, -0.5, 1.0,
    0.5,  0.5, -0.5, 1.0,
    -0.5,  0.5, -0.5, 1.0,
    -0.5, -0.5,  0.5, 1.0,
    0.5, -0.5,  0.5, 1.0,
    0.5,  0.5,  0.5, 1.0,
    -0.5,  0.5,  0.5, 1.0,
};

const GLushort bbox_indices[6][6] = {
    0, 1, 2, 3,
    4, 5, 6, 7,
    0, 4, 1, 5, 2, 6, 3, 7
};

BoundingBox::BoundingBox()
{
    bbox_toWorld = glm::mat4(1.0f);
    collide = false;
    bboxShaderProgram = LoadShaders(BBOX_VERTEX_SHADER_PATH, BBOX_FRAGMENT_SHADER_PATH);
    
    // Create array object and buffers. Remember to delete your buffers when the object is destroyed!
    glGenVertexArrays(1, &bbox_VAO);
    glGenBuffers(1, &bbox_VBO);
    glGenBuffers(1, &bbox_EBO);
    
    // Bind the Vertex Array Object (VAO) first, then bind the associated buffers to it.
    // Consider the VAO as a container for all your buffers.
    glBindVertexArray(bbox_VAO);
    
    // Now bind a VBO to it as a GL_ARRAY_BUFFER. The GL_ARRAY_BUFFER is an array containing relevant data to what
    // you want to draw, such as vertices, normals, colors, etc.
    glBindBuffer(GL_ARRAY_BUFFER, bbox_VBO);
    // glBufferData populates the most recently bound buffer with data starting at the 3rd argument and ending after
    // the 2nd argument number of indices. How does OpenGL know how long an index spans? Go to glVertexAttribPointer.
    glBufferData(GL_ARRAY_BUFFER, sizeof(bbox_vertices), bbox_vertices, GL_STATIC_DRAW);
    // Enable the usage of layout location 0 (check the vertex shader to see what this is)
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0,// This first parameter x should be the same as the number passed into the line "layout (location = x)" in the vertex shader. In this case, it's 0. Valid values are 0 to GL_MAX_UNIFORM_LOCATIONS.
                          4, // This second line tells us how any components there are per vertex. In this case, it's 3 (we have an x, y, and z component)
                          GL_FLOAT, // What type these components are
                          GL_FALSE, // GL_TRUE means the values should be normalized. GL_FALSE means they shouldn't
                          0, // Offset between consecutive indices. Since each of our vertices have 3 floats, they should have the size of 3 floats in between
                          (GLvoid*)0); // Offset of the first vertex's component. In our case it's 0 since we don't pad the vertices array with anything.
    
    // We've sent the vertex data over to OpenGL, but there's still something missing.
    // In what order should it draw those vertices? That's why we'll need a GL_ELEMENT_ARRAY_BUFFER for this.
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, bbox_EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(bbox_indices), bbox_indices, GL_STATIC_DRAW);
    
    // Unbind the currently bound buffer so that we don't accidentally make unwanted changes to it.
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    // Unbind the VAO now so we don't accidentally tamper with it.
    // NOTE: You must NEVER unbind the element array buffer associated with a VAO!
    glBindVertexArray(0);
}

BoundingBox::~BoundingBox()
{
    // Delete previously generated buffers. Note that forgetting to do this can waste GPU memory in a
    // large project! This could crash the graphics driver due to memory leaks, or slow down application performance!
    glDeleteVertexArrays(1, &bbox_VAO);
    glDeleteBuffers(1, &bbox_VBO);
    glDeleteBuffers(1, &bbox_EBO);
}

void BoundingBox::draw(GLuint shaderProgram, glm::mat4 transform)
{
    // Calculate the combination of the model and view (camera inverse) matrices
    glm::mat4 modelview = transform;
    // We need to calcullate this because modern OpenGL does not keep track of any matrix other than the viewport (D)
    // Consequently, we need to forward the projection, view, and model matrices to the shader programs
    // Get the location of the uniform variables "projection" and "modelview"
    glUseProgram(bboxShaderProgram);
    bbox_uProjection = glGetUniformLocation(bboxShaderProgram, "projection");
    bbox_uModelview = glGetUniformLocation(bboxShaderProgram, "modelview");
    // Now send these values to the shader program
    glUniformMatrix4fv(bbox_uProjection, 1, GL_FALSE, &Window::P[0][0]);
    glUniformMatrix4fv(bbox_uModelview, 1, GL_FALSE, &modelview[0][0]);
    
    if (collide == true) {
        glUniform1i(glGetUniformLocation(bboxShaderProgram, "collide"), 1);
    } else {
        glUniform1i(glGetUniformLocation(bboxShaderProgram, "collide"), 0);
    }
    
    // Now draw the cube. We simply need to bind the VAO associated with it.
    glBindVertexArray(bbox_VAO);
    // Tell OpenGL to draw with triangles, using 36 indices, the type of the indices, and the offset to start from
//    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
    glDrawElements(GL_LINE_LOOP, 4, GL_UNSIGNED_SHORT, 0);
    glDrawElements(GL_LINE_LOOP, 4, GL_UNSIGNED_SHORT, (GLvoid*)(4*sizeof(GLushort)));
    glDrawElements(GL_LINES, 8, GL_UNSIGNED_SHORT, (GLvoid*)(8*sizeof(GLushort)));
    // Unbind the VAO when we're done so we don't accidentally draw extra stuff or tamper with its bound buffers
    glBindVertexArray(0);
    glUseProgram(shaderProgram);
}

void BoundingBox::setBoundaries(float max_x, float max_y, float max_z, float min_x, float min_y, float min_z)
{
    this->max_x = max_x;
    this->max_y = max_y;
    this->max_z = max_z;
    this->min_x = min_x;
    this->min_y = min_y;
    this->min_z = min_z;
}
