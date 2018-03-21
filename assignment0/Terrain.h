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
    glm::mat4 toWorldNoSpin;
    glm::mat4 toWorldNoMove;
    bool toWorldNoSpinSet;
    bool toWorldNoMoveSet;
    bool isSwitchModeSet = false;
    
	void draw(GLuint);
	void update();
	void spin(float);

    unsigned int width;
    unsigned int height;
    float offset;
    void move(float delta);
    void loadTerrain(float zOffset);
    void loadTerrain();

	// These variables are needed for the shader program
	GLuint VBO, VAO, EBO, normalbuffer, textureBuffer;
	GLuint uProjection, uModelview;
    void loadTerrainWithHeightMap(const char *filename);
    int heightMapMode;
    
private:
    // get the height value from the heightmap based on the terrain x and z position
    // calculate the normal
    glm::vec3 calculateNormal(int i);
    void getIndices();

};

#endif

