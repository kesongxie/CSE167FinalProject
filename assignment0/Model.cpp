//
//  Model.h
//  assignment0
//
//  Created by Xie kesong on 3/15/18.
//  Copyright © 2018 ___CSE167___. All rights reserved.
//
#include "Model.hpp"
#include "stb_image.h"

using namespace std;
//bool Window::bbox_display;
//vector<BoundingBox*> Window::bbox_vector;


unsigned int TextureFromFile(const char *path, const string &directory, bool gamma)
{
    string filename = string(path);
    //    filename = directory + '/' + filename;
    
    unsigned int textureID;
    glGenTextures(1, &textureID);
    
    int width, height, nrComponents;
    unsigned char *data = stbi_load(filename.c_str(), &width, &height, &nrComponents, 0);
    if (data)
    {
        GLenum format;
        if (nrComponents == 1)
        format = GL_RED;
        else if (nrComponents == 3)
        format = GL_RGB;
        else if (nrComponents == 4)
        format = GL_RGBA;
        
        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
        
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        
        stbi_image_free(data);
    }
    else
    {
        std::cout << "Texture failed to load at path: " << path << std::endl;
        stbi_image_free(data);
    }
    
    return textureID;
}




// draws the model, and thus all its meshes
void Model::Draw(Shader shader)
{
    for(unsigned int i = 0; i < meshes.size(); i++)
    meshes[i].Draw(shader);
    
    if (Window::bbox_display && displayBoundingBox) {
        // toWorld_noRot * transform
        box->draw(shader.ID, Window::V * toWorld * transform);
    }
}

/*  Functions   */
// loads a model with supported ASSIMP extensions from file and stores the resulting meshes in the meshes vector.
void Model::loadModel(string const &path)
{
    // read file via ASSIMP
    Assimp::Importer importer;
    const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_CalcTangentSpace);
    // check for errors
    if(!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) // if is Not Zero
    {
        cout << "ERROR::ASSIMP:: " << importer.GetErrorString() << endl;
        return;
    }
    // retrieve the directory path of the filepath
    //        directory = path.substr(0, path.find_last_of('/'));
    directory = "";
    // process ASSIMP's root node recursively
    processNode(scene->mRootNode, scene);
    
    
    toWorld = glm::mat4(1.0f);
    toWorld_noRot = toWorld;

    // get the bounding box
    float max_x, max_y, max_z, min_x, min_y, min_z;
    // initialize values
    max_x = max_y = max_z = -numeric_limits<float>::max();
    min_x = min_y = min_z = numeric_limits<float>::max();

    // and calculate the maximum dimension of either x, y, or z axis.
    for (int i=0; i<myVertices.size(); i++) {
        glm::vec3 v = myVertices[i];
        if (v.x > max_x) max_x = v.x;
        if (v.x < min_x) min_x = v.x;
        if (v.y > max_y) max_y = v.y;
        if (v.y < min_y) min_y = v.y;
        if (v.z > max_z) max_z = v.z;
        if (v.z < min_z) min_z = v.z;
    }
    
    
    // construct BoundingBox transformation matrix
    box->setBoundaries(max_x + x_coord, max_y + y_coord, max_z + z_coord, min_x + x_coord, min_y + y_coord, min_z + z_coord);
    size = glm::vec3(max_x-min_x, max_y-min_y, max_z-min_z);
    center = glm::vec3((min_x+max_x)/2, (min_y+max_y)/2, (min_z+max_z)/2);
    transform = glm::translate(glm::mat4(1), center) * glm::scale(glm::mat4(1), size);
}

// processes a node in a recursive fashion. Processes each individual mesh located at the node and repeats this process on its children nodes (if any).
void Model::processNode(aiNode *node, const aiScene *scene)
{
    // process each mesh located at the current node
    for(unsigned int i = 0; i < node->mNumMeshes; i++)
    {
        // the node object only contains indices to index the actual objects in the scene.
        // the scene contains all the data, node is just to keep stuff organized (like relations between nodes).
        aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
        meshes.push_back(processMesh(mesh, scene));
    }
    // after we've processed all of the meshes (if any) we then recursively process each of the children nodes
    for(unsigned int i = 0; i < node->mNumChildren; i++)
    {
        processNode(node->mChildren[i], scene);
    }
    
}

Mesh Model::processMesh(aiMesh *mesh, const aiScene *scene)
{
    // data to fill
    vector<Vertex> vertices;
    vector<unsigned int> indices;
    vector<Texture> textures;
    
    // Walk through each of the mesh's vertices
    for(unsigned int i = 0; i < mesh->mNumVertices; i++)
    {
        Vertex vertex;
        glm::vec3 vector; // we declare a placeholder vector since assimp uses its own vector class that doesn't directly convert to glm's vec3 class so we transfer the data to this placeholder glm::vec3 first.
        // positions
        vector.x = mesh->mVertices[i].x;
        vector.y = mesh->mVertices[i].y;
        vector.z = mesh->mVertices[i].z;
        vertex.Position = vector;
        myVertices.push_back(vector);
        
        // normals
        vector.x = mesh->mNormals[i].x;
        vector.y = mesh->mNormals[i].y;
        vector.z = mesh->mNormals[i].z;
        vertex.Normal = vector;
        // texture coordinates
        if(mesh->mTextureCoords[0]) // does the mesh contain texture coordinates?
        {
            glm::vec2 vec;
            // a vertex can contain up to 8 different texture coordinates. We thus make the assumption that we won't
            // use models where a vertex can have multiple texture coordinates so we always take the first set (0).
            vec.x = mesh->mTextureCoords[0][i].x;
            vec.y = mesh->mTextureCoords[0][i].y;
            vertex.TexCoords = vec;
        }
        else
        vertex.TexCoords = glm::vec2(0.0f, 0.0f);
        // tangent
        vector.x = mesh->mTangents[i].x;
        vector.y = mesh->mTangents[i].y;
        vector.z = mesh->mTangents[i].z;
        vertex.Tangent = vector;
        // bitangent
        vector.x = mesh->mBitangents[i].x;
        vector.y = mesh->mBitangents[i].y;
        vector.z = mesh->mBitangents[i].z;
        vertex.Bitangent = vector;
        vertices.push_back(vertex);
    }
    // now wak through each of the mesh's faces (a face is a mesh its triangle) and retrieve the corresponding vertex indices.
    for(unsigned int i = 0; i < mesh->mNumFaces; i++)
    {
        aiFace face = mesh->mFaces[i];
        // retrieve all indices of the face and store them in the indices vector
        for(unsigned int j = 0; j < face.mNumIndices; j++)
        indices.push_back(face.mIndices[j]);
    }
    // process materials
    aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
    // we assume a convention for sampler names in the shaders. Each diffuse texture should be named
    // as 'texture_diffuseN' where N is a sequential number ranging from 1 to MAX_SAMPLER_NUMBER.
    // Same applies to other texture as the following list summarizes:
    // diffuse: texture_diffuseN
    // specular: texture_specularN
    // normal: texture_normalN
    
    // 1. diffuse maps
    vector<Texture> diffuseMaps = loadMaterialTextures(material, aiTextureType_DIFFUSE, "texture_diffuse");
    textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
    // 2. specular maps
    vector<Texture> specularMaps = loadMaterialTextures(material, aiTextureType_SPECULAR, "texture_specular");
    textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
    // 3. normal maps
    std::vector<Texture> normalMaps = loadMaterialTextures(material, aiTextureType_HEIGHT, "texture_normal");
    textures.insert(textures.end(), normalMaps.begin(), normalMaps.end());
    // 4. height maps
    std::vector<Texture> heightMaps = loadMaterialTextures(material, aiTextureType_AMBIENT, "texture_height");
    textures.insert(textures.end(), heightMaps.begin(), heightMaps.end());
    
    // return a mesh object created from the extracted mesh data
    return Mesh(vertices, indices, textures);
}

// checks all material textures of a given type and loads the textures if they're not loaded yet.
// the required info is returned as a Texture struct.
vector<Texture> Model::loadMaterialTextures(aiMaterial *mat, aiTextureType type, string typeName)
{
    vector<Texture> textures;
    for(unsigned int i = 0; i < mat->GetTextureCount(type); i++)
    {
        aiString str;
        mat->GetTexture(type, i, &str);
        // check if texture was loaded before and if so, continue to next iteration: skip loading a new texture
        bool skip = false;
        for(unsigned int j = 0; j < textures_loaded.size(); j++)
        {
            if(std::strcmp(textures_loaded[j].path.data(), str.C_Str()) == 0)
            {
                textures.push_back(textures_loaded[j]);
                skip = true; // a texture with the same filepath has already been loaded, continue to next one. (optimization)
                break;
            }
        }
        if(!skip)
        {   // if texture hasn't been loaded already, load it
            Texture texture;
            texture.id = TextureFromFile(str.C_Str(), this->directory);
            texture.type = typeName;
            texture.path = str.C_Str();
            textures.push_back(texture);
            textures_loaded.push_back(texture);  // store it as texture loaded for entire model, to ensure we won't unnecesery load duplicate textures.
        }
    }
    return textures;
}


void Model::move_x(float value)
{
    x_coord += value;
    toWorld = scaleTransform * glm::translate(glm::mat4(1.0f), glm::vec3(x_coord, y_coord, z_coord)) * glm::rotate(glm::mat4(1.0f), 180 / 180.0f * glm::pi<float>(), glm::vec3(0.0f, 1.0f, 0.0f));
    toWorld_noRot = toWorld;
    center.x += value;
    
    // update bounding box's boundaries
    box->max_x += value;
    box->min_x += value;
}

void Model::move_y(float value)
{
    y_coord += value;
    toWorld = scaleTransform * glm::translate(glm::mat4(1.0f), glm::vec3(x_coord, y_coord, z_coord)) * glm::rotate(glm::mat4(1.0f), 180 / 180.0f * glm::pi<float>(), glm::vec3(0.0f, 1.0f, 0.0f));
    toWorld_noRot = toWorld;
    center.y += value;
    // update bounding box's boundaries
    box->max_y += value;
    box->min_y += value;
}

void Model::move_z(float value)
{
//    z_coord += value;
//    toWorld = glm::translate(glm::mat4(1.0f), glm::vec3(x_coord, y_coord, z_coord));
//    toWorld_noRot = toWorld;
//    // update bounding box's boundaries
//    box->max_z += value;
//    box->min_z += value;
//    if(z_coord > 100) {
//        init_z = z_coord = -1000 + (rand() % 200);
//        init_x = x_coord =  rand() % 800 - 400;
//        init_y = y_coord =  rand() % 100 + 5;
//    }
}

/*
void Model::spin(float deg)
{
    angle += deg;
    if (angle > 360.0f || angle < -360.0f) angle = 0.0f;
    // If you haven't figured it out from the last project, this is how you fix spin's behavior
    toWorld = toWorld * glm::rotate(glm::mat4(1.0f), angle / 180.0f * glm::pi<float>(), glm::vec3(0.0f, 1.0f, 0.0f));
    
    float max_x, max_y, max_z, min_x, min_y, min_z;
    // initialize values
    max_x = max_y = max_z = -numeric_limits<float>::max();
    min_x = min_y = min_z = numeric_limits<float>::max();
    for (int i=0; i<vertices.size(); i++) {
        glm::vec3 old_v = rot_vertices[i];
        glm::vec3 new_v = glm::rotate(old_v, deg / 180.0f * glm::pi<float>(), glm::vec3(0.0f, 1.0f, 0.0f));
        rot_vertices[i] = glm::vec3(new_v.x, new_v.y, new_v.z);
        
        if (new_v.x > max_x) max_x = new_v.x;
        if (new_v.x < min_x) min_x = new_v.x;
        if (new_v.y > max_y) max_y = new_v.y;
        if (new_v.y < min_y) min_y = new_v.y;
        if (new_v.z > max_z) max_z = new_v.z;
        if (new_v.z < min_z) min_z = new_v.z;
    }
    box->setBoundaries(max_x + x_coord, max_y + y_coord, max_z + z_coord, min_x + x_coord, min_y + y_coord, min_z + z_coord);
    size = glm::vec3(max_x-min_x, max_y-min_y, max_z-min_z);
    center = glm::vec3((min_x+max_x)/2, (min_y+max_y)/2, (min_z+max_z)/2);
    transform = glm::translate(glm::mat4(1), center) * glm::scale(glm::mat4(1), size);
}

*/

void Model::setScaleTransform(glm::mat4 scale) {
    scaleTransform = scale;
    toWorld = toWorld * scale;
}


