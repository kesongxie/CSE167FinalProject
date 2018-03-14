#ifndef _TERRAIN_H_
#define _TERRAIN_H_
#define STB_IMAGE_IMPLEMENTATION
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

class Terrain
{
public:
    std::vector<glm::vec3> vertices;
    std::vector<unsigned int> indices;
    std::vector<glm::vec3> normals;
    Terrain(GLint shaderProgram);
	~Terrain();

	glm::mat4 toWorld;
	void draw(GLuint);
	void update();
	void spin(float);

    unsigned int width;
    unsigned int height;

    
	// These variables are needed for the shader program
	GLuint VBO, VAO, EBO, normalbuffer, textureBuffer;
	GLuint uProjection, uModelview;
private:
    // get the height value from the heightmap based on the terrain x and z position
    void loadTerrain();
    // calculate the normal
    glm::vec3 calculateNormal(int i);

};

#endif

