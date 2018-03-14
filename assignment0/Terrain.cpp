#include "Terrain.h"
//#include "Window.h"
#include "main.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>
#include <math.h>
#include "stb_image.h"
#include "PerlinNoise.hpp"

#define HEIGHT_MAP_PATH "/Developer/FinalProject/assignment0/heightmap.bmp"
bool notLoaded = true;

Terrain::Terrain(GLint shaderProgram)
{
    offset = 0;
    this->loadTerrain();

    // Create array object and buffers. Remember to delete your buffers when the object is destroyed!
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);
    glGenBuffers(1, &normalbuffer);
    glGenTextures(1, &textureBuffer);

	
	// Bind the Vertex Array Object (VAO) first, then bind the associated buffers to it.
	// Consider the VAO as a container for all your buffers.
	glBindVertexArray(VAO);

	// Now bind a VBO to it as a GL_ARRAY_BUFFER. The GL_ARRAY_BUFFER is an array containing relevant data to what
	// you want to draw, such as vertices, normals, colors, etc.
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	// glBufferData populates the most recently bound buffer with data starting at the 3rd argument and ending after
	// the 2nd argument number of indices. How does OpenGL know how long an index spans? Go to glVertexAttribPointer.
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3), &vertices[0], GL_STATIC_DRAW);
	// Enable the usage of layout location 0 (check the vertex shader to see what this is)
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0,// This first parameter x should be the same as the number passed into the line "layout (location = x)" in the vertex shader. In this case, it's 0. Valid values are 0 to GL_MAX_UNIFORM_LOCATIONS.
		3, // This second line tells us how any components there are per vertex. In this case, it's 3 (we have an x, y, and z component)
		GL_FLOAT, // What type these components are
		GL_FALSE, // GL_TRUE means the values should be normalized. GL_FALSE means they shouldn't
		0, // Offset between consecutive indices. Since each of our vertices have 3 floats, they should have the size of 3 floats in between
		(GLvoid*)0); // Offset of the first vertex's component. In our case it's 0 since we don't pad the vertices array with anything.
    
    // ----- NORMAL --------
    // Now bind a VBO to it as a GL_ARRAY_BUFFER. The GL_ARRAY_BUFFER is an array containing relevant data to what
    // you want to draw, such as vertices, normals, colors, etc.
    glBindBuffer(GL_ARRAY_BUFFER, normalbuffer); // this is the actual vertices information
    glBufferData(GL_ARRAY_BUFFER, normals.size() * sizeof(glm::vec3), &normals[0], GL_STATIC_DRAW);
    
    // Enable the usage of layout location 0 (check the vertex shader to see what this is)
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
    
    
    
     // ----- TEXTURE --------
    // bind the texture
    glBindTexture(GL_TEXTURE_2D, textureBuffer);
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), (GLvoid*)0);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    
    int width, height, nrChannels;
    unsigned char *data = stbi_load("/Developer/FinalProject/assignment0/cloud_texture.jpg", &width, &height, &nrChannels, 0);
    if (data)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        stbi_image_free(data);
        glGenerateMipmap(GL_TEXTURE_2D);
    } else {
        std:: cout << "failed to load image" << std::endl;
    }
    
    glUniform1i(glGetUniformLocation(shaderProgram, "texture1"), 0);


	// We've sent the vertex data over to OpenGL, but there's still something missing.
	// In what order should it draw those vertices? That's why we'll need a GL_ELEMENT_ARRAY_BUFFER for this.
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int) , &indices[0], GL_STATIC_DRAW);

	// Unbind the currently bound buffer so that we don't accidentally make unwanted changes to it.
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	// Unbind the VAO now so we don't accidentally tamper with it.
	// NOTE: You must NEVER unbind the element array buffer associated with a VAO!
	glBindVertexArray(0);
}

Terrain::~Terrain()
{
	// Delete previously generated buffers. Note that forgetting to do this can waste GPU memory in a 
	// large project! This could crash the graphics driver due to memory leaks, or slow down application performance!
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &EBO);
}

void Terrain::draw(GLuint shaderProgram)
{ 
	// Calculate the combination of the model and view (camera inverse) matrices
	glm::mat4 modelview = Window::V * toWorld;
	// We need to calcullate this because modern OpenGL does not keep track of any matrix other than the viewport (D)
	// Consequently, we need to forward the projection, view, and model matrices to the shader programs
	// Get the location of the uniform variables "projection" and "modelview"
	uProjection = glGetUniformLocation(shaderProgram, "projection");
	uModelview = glGetUniformLocation(shaderProgram, "modelview");
	// Now send these values to the shader program
	glUniformMatrix4fv(uProjection, 1, GL_FALSE, &Window::P[0][0]);
	glUniformMatrix4fv(uModelview, 1, GL_FALSE, &modelview[0][0]);
	// Now draw the cube. We simply need to bind the VAO associated with it.
	glBindVertexArray(VAO);
    
    // Filled
//    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    // Wireframe
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    glBindTexture(GL_TEXTURE_2D, textureBuffer);


    
	// Tell OpenGL to draw with triangles, using 36 indices, the type of the indices, and the offset to start from
	glDrawElements(GL_TRIANGLE_STRIP, indices.size(), GL_UNSIGNED_INT, 0);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    
    
    
    // Unbind the VAO when we're done so we don't accidentally draw extra stuff or tamper with its bound buffers
	glBindVertexArray(0);
}


void Terrain::loadTerrain() {
    loadTerrain(0);
}


void Terrain::loadTerrain(float zOffset) {
//    SDL_Surface* img = SDL_LoadBMP(HEIGHT_MAP_PATH);
    
    toWorld = glm::translate(glm::mat4(1.0f), glm::vec3(0, -20, -20.0));
    width = 100;
    height = 10000;
    float scale = 2;
//    float offset = 2;
//
//    uint8_t *pixels = (uint8_t *) img->pixels;
//    float scale_x = ((float) img->w) / (width - 1);
//    float scale_z = ((float) img->h) / (height - 1);

    
    // (double _persistence, double _frequency, double _amplitude, int _octaves, int _randomseed);
    PerlinNoise *noise = new PerlinNoise(1.1, 0.1, 2.0, 3, 4);
    for(unsigned int z = 0; z < height; z++) {
        for(unsigned int x = 0; x < width; x++) {
            glm::vec3 point = glm::vec3(x, 0, z);
            
            /*
            // get the height
            int img_x = (int) truncf(x * scale_x);
            int img_y = (int) truncf(z * scale_z);
            float h = pixels[img_y * img->pitch + img_x * 4];
            
            
            // Normalize height to [-1, 1]
            h = h / 127.5 - 1.0f;
            
            // Apply scale
            h *= scale;
            
            // Apply height offset
            h += offset;
            point.y = h;
            */
            point.y = noise->GetHeight(x, z) * 4.0;
//            point.y = rand() % 2 - 1;
            vertices.push_back(point);
        }
    }
    

    for(int i = 0; i < vertices.size(); i++) {
        vertices[i].x -= width / 2.0;
        vertices[i].z -= height / 2.0;
    }
    
    for(int i = 0; i < vertices.size(); i++) {
        vertices[i].x *= scale;
        vertices[i].z *= scale;
    }
    getIndices();
}


void Terrain::getIndices() {
    for(unsigned int z = 1; z < height; z++) {
        unsigned int curIndex =  (z - 1) * width;
        indices.push_back(curIndex);
        indices.push_back(curIndex);
        bool down = true; //true => down, false => top right
        for(unsigned i = 1; i < 2 * width; i++) {
            if(down) {
                curIndex = curIndex + width;
                indices.push_back(curIndex);
            } else {
                curIndex = curIndex + 1 - width;
                indices.push_back(curIndex);
            }
            down = !down;
            unsigned long lastIndex = indices.size();
            
            if (lastIndex > 2) {
                // get the last three as the points for the triangles
                glm::vec3 point_0 = vertices[(indices[lastIndex - 1])];
                glm::vec3 point_1 = vertices[(indices[lastIndex - 2])];
                glm::vec3 point_2 = vertices[(indices[lastIndex - 3])];
                glm::vec3 edge_1 = glm::vec3(point_2.x - point_1.x, point_2.y - point_1.y, point_2.z - point_1.z);
                glm::vec3 edge_2 = glm::vec3(point_1.x - point_0.x, point_1.y - point_0.y, point_1.z - point_0.z);
                normals.push_back(glm::normalize(glm::cross(edge_1, edge_2)));
            }
            
        }
        indices.push_back(curIndex);
    }
}


glm::vec3 Terrain::calculateNormal(int i) {
    return glm::vec3(1.0f);
    
    /*
    int left = i - 1;
    int right = i + 1;
    int top = i - width;
    int bottom = i + width;
    
    float hl = 0;
    float hr = 0;
    float ht = 0;
    float hb = 0;
    if( left >= 0 && left < width * length) {
        hl = vertices[left].y;
    }
    if( right >= 0 && right < width * length) {
        hr = vertices[right].y;
    }
    if( top >= 0 && top < width * length) {
        ht = vertices[top].y;
    }
    if( bottom >= 0 && bottom < width * length) {
        hb = vertices[bottom].y;
    }
    int y = rand() % 4;
    float mag = sqrt(pow(hl - hr, 2) + pow(y, 2) + pow(hb - ht, 2));
    return glm::vec3((hl - hr) / mag, (y) / mag, (hb - ht) / mag);*/
}

void Terrain::update()
{
//    spin(1.0f);
    move(1.0f);
    
}

void Terrain::spin(float deg)
{
	// If you haven't figured it out from the last project, this is how you fix spin's behavior
	toWorld = toWorld * glm::rotate(glm::mat4(1.0f), 1.0f / 180.0f * glm::pi<float>(), glm::vec3(0.0f, 1.0f, 0.0f));
}

void Terrain::move(float delta) {
    offset -= delta;
    toWorld = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, delta)) * toWorld;

}

