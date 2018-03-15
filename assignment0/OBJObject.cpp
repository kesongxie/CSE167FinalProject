#include "OBJObject.h"
#include "Window.h"

using namespace std;

bool Window::bbox_display;
vector<BoundingBox*> Window::bbox_vector;

OBJObject::OBJObject(const char *filepath)
{
    box = new BoundingBox();
    angle = 0.0f;
    init_x = x_coord = 0;
    init_y = y_coord = 0;
    if(strcmp(filepath, "Asteroid.obj") == 0){
        Window::bbox_vector.push_back(box); // to compare all bounding boxes' boundaries
        init_z = z_coord = -500;
        toWorld = glm::mat4(1.0f);
        toWorld = toWorld * glm::translate(glm::mat4(1.0f), glm::vec3(x_coord, y_coord, z_coord));
        toWorld_noRot = toWorld;
    } else {
        init_z = z_coord = 0;
        toWorld = glm::rotate(glm::mat4(1.0f), 180 / 180.0f * glm::pi<float>(), glm::vec3(0.0f, 1.0f, 0.0f));
        toWorld = toWorld * glm::translate(glm::mat4(1.0f), glm::vec3(x_coord, y_coord, z_coord));
        toWorld_noRot = glm::translate(glm::mat4(1.0f), glm::vec3(x_coord, y_coord, z_coord));
    }
    
    parse(filepath);
    
    // Create array object and buffers. Remember to delete your buffers when the object is destroyed!
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &NBO);
    glGenBuffers(1, &EBO);
    
    // Bind the Vertex Array Object (VAO) first, then bind the associated buffers to it.
    // Consider the VAO as a container for all your buffers.
    glBindVertexArray(VAO);
    
    // Now bind a VBO to it as a GL_ARRAY_BUFFER. The GL_ARRAY_BUFFER is an array containing relevant data to what
    // you want to draw, such as vertices, normals, colors, etc.
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    // glBufferData populates the most recently bound buffer with data starting at the 3rd argument and ending after
    // the 2nd argument number of indices. How does OpenGL know how long an index spans? Go to glVertexAttribPointer.
    glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * vertices.size(), vertices.data(), GL_STATIC_DRAW);
    // Enable the usage of layout location 0 (check the vertex shader to see what this is)
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
    
    // Use layout location 1 for the normals
    glBindBuffer(GL_ARRAY_BUFFER, NBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * normals.size(), normals.data(), GL_STATIC_DRAW);
    glEnableVertexAttribArray(1); // This allows usage of layout location 1 in the vertex shader
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), 0);
    
    // We've sent the vertex data over to OpenGL, but there's still something missing.
    // In what order should it draw those vertices? That's why we'll need a GL_ELEMENT_ARRAY_BUFFER for this.
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * indices.size(), &indices[0], GL_STATIC_DRAW);
    
    // Unbind the currently bound buffer so that we don't accidentally make unwanted changes to it.
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    // Unbind the VAO now so we don't accidentally tamper with it.
    // NOTE: You must NEVER unbind the element array buffer associated with a VAO!
    glBindVertexArray(0);
}

OBJObject::~OBJObject()
{
    // Delete previously generated buffers. Note that forgetting to do this can waste GPU memory in a
    // large project! This could crash the graphics driver due to memory leaks, or slow down application performance!
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &NBO);
    glDeleteBuffers(1, &EBO);
}

void OBJObject::parse(const char *filepath)
{
    //TODO parse the OBJ file
    // Populate the face indices, vertices, and normals vectors with the OBJ Object data
    
    FILE* fp;
    float r,g,b;
    unsigned int fx, fy, fz, ignore;
    // initialize values
    max_x = max_y = max_z = -numeric_limits<float>::max();
    min_x = min_y = min_z = numeric_limits<float>::max();
    
    fp = fopen(filepath, "r");
    if (fp==NULL) {
        cerr << "error loading file" << endl;
        exit(-1);
    }
    
    while (1) {
        char lineHeader[128];
        /* read the first word of the line */
        int res = fscanf(fp, "%s", lineHeader);
        /* if reaches end of file, break loop */
        if (res == EOF) {
            break;
        }
        
        /* only parse lines starting with "v", "vn", or "f" */
        if ( strcmp( lineHeader, "v" ) == 0 ){
            glm::vec3 vertex;
            fscanf(fp, "%f %f %f %f %f %f\n", &vertex.x, &vertex.y, &vertex.z, &r, &g, &b );
            vertices.push_back(vertex);
        } else if( strcmp( lineHeader, "vn" ) == 0) {
            glm::vec3 normal;
            fscanf(fp, "%f %f %f\n", &normal.x, &normal.y, &normal.z );
            normals.push_back(normal);
        } else if( strcmp( lineHeader, "f" ) == 0) {
            if(strcmp(filepath, "Dragon.obj") == 0 || strcmp(filepath, "Asteroid.obj") == 0) {
                fscanf(fp, "%d/%d/%d %d/%d/%d %d/%d/%d\n", &fx, &ignore, &ignore, &fy, &ignore, &ignore, &fz, &ignore, &ignore);
            } else {
                fscanf(fp, "%d//%d %d//%d %d//%d\n", &fx, &ignore, &fy, &ignore, &fz, &ignore);
            }
            // indices are zero-based numbering
            indices.push_back(fx - 1);
            indices.push_back(fy - 1);
            indices.push_back(fz - 1);
        }
    }
    
    // get a copy of vertices, keep track of all vertex postions after rotation
    rot_vertices = vertices;
    dragon_vertices = vertices;
    
    // iterate thru all vertices to find maximum and minimum values of x,y,z coordinates,
    // and calculate the maximum dimension of either x, y, or z axis.
    for (int i=0; i<vertices.size(); i++) {
        glm::vec3 v = vertices[i];
        if (strcmp(filepath, "Dragon.obj") == 0){
            glm::vec3 new_v = glm::rotate(v, 180.0f / 180.0f * glm::pi<float>(), glm::vec3(0.0f, 1.0f, 0.0f));
            dragon_vertices[i] = glm::vec3(new_v.x, new_v.y, new_v.z);
            
            if (new_v.x > max_x) max_x = new_v.x;
            if (new_v.x < min_x) min_x = new_v.x;
            if (new_v.y > max_y) max_y = new_v.y;
            if (new_v.y < min_y) min_y = new_v.y;
            if (new_v.z > max_z) max_z = new_v.z;
            if (new_v.z < min_z) min_z = new_v.z;
        } else {
            if (v.x > max_x) max_x = v.x;
            if (v.x < min_x) min_x = v.x;
            if (v.y > max_y) max_y = v.y;
            if (v.y < min_y) min_y = v.y;
            if (v.z > max_z) max_z = v.z;
            if (v.z < min_z) min_z = v.z;
        }
    }

    //    max_dimension = max(max((max_x-min_x)/2, (max_y-min_y)/2), (max_z-min_z)/2);
    //
    //    // map vertices into a 2x2x2 cube
    //    for (int i=0; i<vertices.size(); i++) {
    //        glm::vec3 v = vertices[i];
    //        vertices[i].x = v.x/max_dimension;
    //        vertices[i].y = v.y/max_dimension;
    //        vertices[i].z = v.z/max_dimension;
    //    }
    
    init_max_x = max_x;
    init_min_x = min_x;
    init_max_y = max_y;
    init_min_y = min_y;
    init_max_z = max_z;
    init_min_z = min_z;
    
    // construct BoundingBox transformation matrix
    box->setBoundaries(max_x + x_coord, max_y + y_coord, max_z + z_coord, min_x + x_coord, min_y + y_coord, min_z + z_coord);
    size = glm::vec3(max_x-min_x, max_y-min_y, max_z-min_z);
    center = glm::vec3((min_x+max_x)/2, (min_y+max_y)/2, (min_z+max_z)/2);
    transform = glm::translate(glm::mat4(1), center) * glm::scale(glm::mat4(1), size);
    
    fclose(fp);
}

void OBJObject::draw(GLuint shaderProgram)
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
    // Tell OpenGL to draw with triangles, using 36 indices, the type of the indices, and the offset to start from
    glDrawElements(GL_TRIANGLES, (GLsizei)indices.size(), GL_UNSIGNED_INT, 0);
    // Unbind the VAO when we're done so we don't accidentally draw extra stuff or tamper with its bound buffers
    glBindVertexArray(0);
    
    if (Window::bbox_display) {
        box->draw(shaderProgram, Window::V * toWorld_noRot * transform);
    }
}

void OBJObject::followCursor(float x_val, float y_val)
{
    move_x(x_val);
    move_y(y_val);
}

void OBJObject::move_x(float value)
{
    x_coord += value;
    toWorld = glm::translate(glm::mat4(1.0f), glm::vec3(x_coord, y_coord, z_coord)) * glm::rotate(glm::mat4(1.0f), 180 / 180.0f * glm::pi<float>(), glm::vec3(0.0f, 1.0f, 0.0f));
    toWorld_noRot = glm::translate(glm::mat4(1.0f), glm::vec3(x_coord, y_coord, z_coord));
    // update bounding box's boundaries
    box->max_x += value;
    box->min_x += value;
}

void OBJObject::move_y(float value)
{
    y_coord += value;
    toWorld = glm::translate(glm::mat4(1.0f), glm::vec3(x_coord, y_coord, z_coord)) * glm::rotate(glm::mat4(1.0f), 180 / 180.0f * glm::pi<float>(), glm::vec3(0.0f, 1.0f, 0.0f));
    toWorld_noRot = glm::translate(glm::mat4(1.0f), glm::vec3(x_coord, y_coord, z_coord));
    // update bounding box's boundaries
    box->max_y += value;
    box->min_y += value;
}

void OBJObject::move_z(float value)
{
    z_coord += value;
    toWorld = glm::translate(glm::mat4(1.0f), glm::vec3(x_coord, y_coord, z_coord)) * glm::scale(glm::mat4(1.0f), glm::vec3(0.5f));
    toWorld_noRot = toWorld;
    // update bounding box's boundaries
    box->max_z += value;
    box->min_z += value;
    
    if(z_coord == 70){
        // reset z-coordinate
        z_coord = init_z;
        // reset boudning box
        box->setBoundaries(init_max_x + x_coord, init_max_y + y_coord, init_max_z + z_coord, init_min_x + x_coord, init_min_y + y_coord, init_min_z + z_coord);
        size = glm::vec3(init_max_x-init_min_x, init_max_y-init_min_y, init_max_z-init_min_z);
        center = glm::vec3((init_min_x+init_max_x)/2, (init_min_y+init_max_y)/2, (init_min_z+init_max_z)/2);
        transform = glm::translate(glm::mat4(1), center) * glm::scale(glm::mat4(1), size);
        // reset rotation
        toWorld = toWorld * glm::rotate(glm::mat4(1.0f), 0 / 180.0f * glm::pi<float>(), glm::vec3(0.0f, 1.0f, 0.0f)) * glm::scale(glm::mat4(1.0f), glm::vec3(0.5f));
    }
}

void OBJObject::spin(float deg)
{
    angle += deg;
    if (angle > 360.0f || angle < -360.0f) angle = 0.0f;
    // If you haven't figured it out from the last project, this is how you fix spin's behavior
    toWorld = toWorld * glm::rotate(glm::mat4(1.0f), angle / 180.0f * glm::pi<float>(), glm::vec3(0.0f, 1.0f, 0.0f)) * glm::scale(glm::mat4(1.0f), glm::vec3(0.5f));
    
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
    
    float scale = 0.5f;
    box->setBoundaries(max_x*scale + x_coord, max_y*scale + y_coord, max_z*scale + z_coord, min_x*scale + x_coord, min_y*scale + y_coord, min_z*scale + z_coord);
    size = glm::vec3(max_x*scale-min_x*scale, max_y*scale-min_y*scale, max_z*scale-min_z*scale);
    center = glm::vec3((min_x*scale+max_x*scale)/2, (min_y*scale+max_y*scale)/2, (min_z*scale+max_z*scale)/2);
    transform = glm::translate(glm::mat4(1), center) * glm::scale(glm::mat4(1), size);
}
