//
//  Model.h
//  assignment0
//
//  Created by Xie kesong on 3/15/18.
//  Copyright © 2018 ___CSE167___. All rights reserved.
//

#ifndef Model_h
#define Model_h

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "Mesh.h"
#include "Shader_class.h"

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <map>
#include <vector>
#include "BoundingBox.h"
#include "Window.h"


using namespace std;

unsigned int TextureFromFile(const char *path, const string &directory, bool gamma = false);

class Model
{
public:
    /*  Model Data */
    vector<Texture> textures_loaded;    // stores all the textures loaded so far, optimization to make sure textures aren't loaded more than once.
    vector<Mesh> meshes;
    string directory;
    bool gammaCorrection;
    BoundingBox * box;
    glm::mat4 toWorld;
    glm::mat4 toWorld_noRot;
    std::vector<glm::vec3> myVertices;
    glm::vec3 size;
    glm::vec3 center;
    glm::mat4 transform;
    float originZ;
    bool displayBoundingBox;
    
    Model(string const &path, bool gamma = false) : gammaCorrection(gamma)
    {
        displayBoundingBox = true;
        box = new BoundingBox();
        loadModel(path);
    }
    
    // draws the model, and thus all its meshes
    void Draw(Shader shader);
    
    
    // movements
    void move_x(float);
    void move_y(float);
    void move_z(float);
    void spin(float);
    void setScaleTransform(glm::mat4 scale);
    
private:
    /* object's coordinates in window when initialized */
    float init_x;
    float init_y;
    float init_z;
    /* object's current coordinates in window */
    float x_coord;
    float y_coord;
    float z_coord;
    glm::mat4 scaleTransform;
    
    /*  Functions   */
    // loads a model with supported ASSIMP extensions from file and stores the resulting meshes in the meshes vector.
    void loadModel(string const &path);
    
    // processes a node in a recursive fashion. Processes each individual mesh located at the node and repeats this process on its children nodes (if any).
    void processNode(aiNode *node, const aiScene *scene);
    
    Mesh processMesh(aiMesh *mesh, const aiScene *scene);
    vector<Texture> loadMaterialTextures(aiMaterial *mat, aiTextureType type, string typeName);
    
};


#endif /* Model_h */


