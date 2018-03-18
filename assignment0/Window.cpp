#include "window.h"
#include "Terrain.h"
#include "Skybox.hpp"
#include "OBJObject.h"
#include "PerlinNoise.hpp"
#include <chrono>
#include <thread>
#include "Model.hpp"
#include "Shader_class.h"
#include "Bullet.hpp"
#include "AudioManager.hpp"
#define EXPLODE_MUSIC_PATH "explosion.wav"


const char* window_title = "GLFW Starter Project";
Terrain * terrain;
Skybox * skybox;
GLint shaderProgram;
GLint skyboxshaderProgram;
GLint bulletshaderProgram;
GLint objShaderProgram;
Shader *ourShader;
bool shaking = false;

// obj
OBJObject * obj;
OBJObject * obj2;
OBJObject * obj3;
OBJObject * obj4;
OBJObject * obj5;
OBJObject * obj6;


// load models
Model *SS1;
Model *UFO;
Model *fighter;
Model *character;


// trackball variables
double prev_x, prev_y;
double curr_x, curr_y;
glm::vec3 curPoint, lastPoint;
bool rot = false;

glm::vec3 rotAxis;
float rotAngle;
float delta = 0;
float timeLast = 0;
float degree = 0;
float angle = 0;


// On some systems you need to change this to the absolute path
#define VERTEX_SHADER_PATH "./shader.vert"
#define FRAGMENT_SHADER_PATH "./shader.frag"

#define BULLET_VERTEX_SHADER_PATH "./bullet-shader.vert"
#define BULLET_FRAGMENT_SHADER_PATH "./bullet-shader.frag"


#define SKYBOX_VERTEX_SHADER_PATH "./skybox.vert"
#define SKYBOX_FRAGMENT_SHADER_PATH "./skybox.frag"

#define OBJ_VERTEX_SHADER_PATH "./android.vert"
#define OBJ_FRAGMENT_SHADER_PATH "./android.frag"


// Default camera parameters
glm::vec3 cam_pos(0.0f, 0.0f, 60.0f);		// e  | Position of camera
glm::vec3 cam_look_at(0.0f, 0.0f, 0.0f);	// d  | This is where the camera looks at
glm::vec3 cam_up(0.0f, 1.0f, 0.0f);			// up | What orientation "up" is

glm::vec3 cam_pos_before_collison = cam_pos;

int Window::width;
int Window::height;

glm::mat4 Window::P;
glm::mat4 Window::V;
PerlinNoise *noise;

#define MAX_BULLET_NUM 5
#define MAX_ASTEROID_NUM 6

std::vector<Bullet> bullets;
std::vector<OBJObject *> asteroids;


void Window::initialize_objects()
{
    
    noise = new PerlinNoise(1.1, 0.1, 2.0, 3, 4);

	// Load the shader program. Make sure you have the correct filepath up top
	shaderProgram = LoadShaders(VERTEX_SHADER_PATH, FRAGMENT_SHADER_PATH);

    skyboxshaderProgram = LoadShaders(SKYBOX_VERTEX_SHADER_PATH, SKYBOX_FRAGMENT_SHADER_PATH);
    objShaderProgram = LoadShaders(OBJ_VERTEX_SHADER_PATH, OBJ_FRAGMENT_SHADER_PATH);
    ourShader = new Shader("./modelLoading.vs", "./modelLoading.frag");

    skybox = new Skybox(skyboxshaderProgram);
    terrain = new Terrain(shaderProgram);
    
    for(unsigned int i = 0; i < MAX_ASTEROID_NUM; i++) {
        asteroids.push_back(new OBJObject("Asteroid.obj"));
    }
    
    SS1 = new Model("Low_poly_UFO.obj");
    SS1->displayBoundingBox = false;
    UFO = new Model("Low_poly_UFO.obj");
    UFO->setScaleTransform(glm::scale(glm::mat4(1.0f), glm::vec3(0.6f, 0.6f, 0.6f)));
    fighter = new Model("Viper-mk-IV-fighter.obj");
    character = UFO;
    UFO->move_y(-40);
    
    for(unsigned int i = 0; i < MAX_BULLET_NUM; i++) {
        bullets.push_back(Bullet(new OBJObject("Sphere.obj"), UFO));
    }
    

}

void drawActiveBullet() {
    for(unsigned int i = 0; i < MAX_BULLET_NUM; i++) {
        if(bullets[i].isActive) {
            bullets[i].draw();
        }
    }
}

void recycleBullet() {
    for(unsigned int i = 0; i < MAX_BULLET_NUM; i++) {
        bullets[i].recycleIfNeeded();
    }
}

void moveActiveBullet() {
    for(unsigned int i = 0; i < MAX_BULLET_NUM; i++) {
        if(bullets[i].isActive) {
            glm::vec3 diretion(0.0, 3.0, -20.0);
            bullets[i].move(diretion);
        }
    }
}

void shootBullet() {
    for(unsigned int i = 0; i < MAX_BULLET_NUM; i++) {
        // find the next not active bullet and shoot it
        if(!bullets[i].isActive) {
            bullets[i].isActive = true;
            AudioManager::openALPlay("44.wav", false);
            break;
        }
    }
}

// Treat this as a destructor function. Delete dynamically allocated memory here.
void Window::clean_up()
{
	delete(terrain);
    delete(skybox);
    delete(obj);
    delete(ourShader);
    delete(SS1);
    delete(UFO);
    delete(fighter);
	glDeleteProgram(shaderProgram);
    glDeleteProgram(skyboxshaderProgram);
    glDeleteProgram(objShaderProgram);
}

GLFWwindow* Window::create_window(int width, int height)
{
	// Initialize GLFW
	if (!glfwInit())
	{
		fprintf(stderr, "Failed to initialize GLFW\n");
		return NULL;
	}

	// 4x antialiasing
	glfwWindowHint(GLFW_SAMPLES, 4);

#ifdef __APPLE__ // Because Apple hates comforming to standards
	// Ensure that minimum OpenGL version is 3.3
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	// Enable forward compatibility and allow a modern OpenGL context
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

	// Create the GLFW window
	GLFWwindow* window = glfwCreateWindow(width, height, window_title, NULL, NULL);

	// Check if the window could not be created
	if (!window)
	{
		fprintf(stderr, "Failed to open GLFW window.\n");
		fprintf(stderr, "Either GLFW is not installed or your graphics card does not support modern OpenGL.\n");
		glfwTerminate();
		return NULL;
	}

	// Make the context of the window
	glfwMakeContextCurrent(window);

	// Set swap interval to 1
	glfwSwapInterval(1);

	// Get the width and height of the framebuffer to properly resize the window
	glfwGetFramebufferSize(window, &width, &height);
	// Call the resize callback to make sure things get drawn immediately
	Window::resize_callback(window, width, height);

	return window;
}

void Window::resize_callback(GLFWwindow* window, int width, int height)
{
#ifdef __APPLE__
	glfwGetFramebufferSize(window, &width, &height); // In case your Mac has a retina display
#endif
	Window::width = width;
	Window::height = height;
	// Set the viewport size. This is the only matrix that OpenGL maintains for us in modern OpenGL!
	glViewport(0, 0, width, height);

	if (height > 0)
	{
		P = glm::perspective(45.0f, (float)width / (float)height, 0.1f, 1000.0f);
		V = glm::lookAt(cam_pos, cam_look_at, cam_up);
	}
}

void Window::idle_callback()
{
	// Call the update function the cube
    // modify the camera position
    
    for(unsigned int i = 0; i < MAX_ASTEROID_NUM; i++) {
        asteroids[i]->move_z(20 + rand() % 40);
        asteroids[i]->spin(1 + rand() % 3);
    }

    
    moveActiveBullet();
    recycleBullet();
    

    // Iterate thru bounding boxes vector, use AABB to detect if any collide with dragon's boudning box. If any collide with dragon, set the colliding boxes’ collide flags to true (colliding boxes will be colored red via fragment shader).
    BoundingBox * boxA = UFO->box;
    
    for(auto iter = bbox_vector.begin(); iter != bbox_vector.end(); iter++){
        BoundingBox * boxB = *iter;
        if (checkCollision(boxA, boxB)) {
            boxA->collide = true;
            boxB->collide = true;
            // shaker the camera
            float offset = (float)(rand() % 2 - 0.5) * 4 / (rand() % 10 - 0.5);
//            cam_pos = glm::vec3(cam_pos.x + offset, cam_pos.y + offset, cam_pos.z);
//            V = glm::lookAt(cam_pos, cam_look_at, cam_up);
//            timeLast += 1;
        } else {
            boxA->collide = false;
            boxB->collide = false;
            if(timeLast > 5) {
                timeLast = 0;
                V = glm::lookAt(cam_pos_before_collison, cam_look_at, cam_up);
            }
        }
    }
    
    
 for(unsigned int i = 0; i < MAX_ASTEROID_NUM; i++) {
     BoundingBox * boxB = asteroids[i]->box;
        for(auto iter = bullets.begin(); iter != bullets.end(); iter++){
            BoundingBox * boxA =  iter->obj->box;
            if (checkCollision(boxA, boxB)) {
                boxA->collide = true;
                boxB->collide = true;
                asteroids[i]->resetZ();
                // shaker the camera
//                float offset = (float)(rand() % 2 - 0.5) * 4 / (rand() % 10 - 0.5);
//                cam_pos = glm::vec3(cam_pos.x + offset, cam_pos.y + offset, cam_pos.z);
//                V = glm::lookAt(cam_pos, cam_look_at, cam_up);
                timeLast += 1;
                std:: cout << "bingo" << std::endl;
                AudioManager::openALPlay(EXPLODE_MUSIC_PATH, false);
                
                
            } else {
                boxA->collide = false;
                boxB->collide = false;
                if(timeLast > 5) {
                    timeLast = 0;
                    V = glm::lookAt(cam_pos_before_collison, cam_look_at, cam_up);
                }
            }
        }
    }
    
    
    
    // check collision between bullet and asteroid
    
    
    terrain->update();
}

void Window::display_callback(GLFWwindow* window)
{
	// Clear the color and depth buffers
//    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    
//    glDepthMask(d);

    // SKYBOX
    glUseProgram(skyboxshaderProgram);
    glDepthFunc(GL_LEQUAL);  // change depth function so depth test passes when values are equal to depth buffer's content

    // Now send these values to the shader program
    glUniformMatrix4fv(glGetUniformLocation(skyboxshaderProgram, "projection"), 1, GL_FALSE, &Window::P[0][0]);
    degree += 1.0f;
    glm::mat4 model = Window::V * glm::rotate(glm::mat4(1.0f), degree / 180.0f * glm::pi<float>(), glm::vec3(0.0f, 1.0f, 0.0f));
    glUniformMatrix4fv(glGetUniformLocation(skyboxshaderProgram, "view"), 1, GL_FALSE, &model[0][0]);
    skybox->render();

    
    // --- TERRAIN
    // Use the shader of programID
    glDisable(GL_CULL_FACE);
    glUseProgram(shaderProgram);
    
    // Render the cube
    terrain->draw(shaderProgram);
    drawActiveBullet();
    

    
    
    // DRAGON
    // render the dragon and the rock
    glUseProgram(objShaderProgram);
    for(unsigned int i = 0; i < MAX_ASTEROID_NUM; i++) {
        asteroids[i]->draw(objShaderProgram);
    }
    
    
    // don't forget to enable shader before setting uniforms
    ourShader->use();
    
    // view/projection transformations
    // Now send these values to the shader program
    glUniformMatrix4fv(glGetUniformLocation(ourShader->ID, "projection"), 1, GL_FALSE, &Window::P[0][0]);
    glUniformMatrix4fv(glGetUniformLocation(ourShader->ID, "view"), 1, GL_FALSE, &Window::V[0][0]);
    
   
    // render the loaded model
    // this is the main chracter
    angle -= 1.0f;
    
    glm::mat4 transform = glm::rotate(UFO->toWorld, angle / 180.0f * glm::pi<float>(), glm::vec3(0.0f, 1.0f, 0.0f)) * glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f)); // translate it down so it's at the center of the scene
    glUniformMatrix4fv(glGetUniformLocation(ourShader->ID, "model"), 1, GL_FALSE, &transform[0][0]);
    UFO->Draw(*ourShader);
    
    
    // render the UFO at the top of the scene
    glm::mat4 transform_ss1 =  glm::rotate(glm::mat4(1.0f), 30 / 180.0f * glm::pi<float>(), glm::vec3(0.0f, 1.0f, 0.0f)) * glm::translate(glm::mat4(1.0f), glm::vec3(-200.0f, -20.0f, -400.0f)); // translate it down so it's at the center of the scene
    transform_ss1 = glm::scale(transform_ss1, glm::vec3(20.0f, 20.0f, 20.0f));
    glUniformMatrix4fv(glGetUniformLocation(ourShader->ID, "model"), 1, GL_FALSE, &transform_ss1[0][0]);
    SS1->Draw(*ourShader);
    
    

	// Gets events, including input such as keyboard and mouse or window resizing
}


void Window::key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    // Check for a key press
    if (action == GLFW_PRESS)
    {
        // Check if escape was pressed
        if (key == GLFW_KEY_ESCAPE)
        {
            // Close the window. This causes the program to also terminate.
            glfwSetWindowShouldClose(window, GL_TRUE);
        }
        // debug: display/hide bounding box
        if (key == GLFW_KEY_D)
        {
            Window::bbox_display = !Window::bbox_display;
        }
        if (key == GLFW_KEY_SPACE) {
            shootBullet();
        }
        
    }
        if (key == GLFW_KEY_UP)
        {
            character->move_y(2.0f);
        }
        if (key == GLFW_KEY_DOWN)
        {
            character->move_y(-2.0f);
        }
        if (key == GLFW_KEY_LEFT)
        {
            character->move_x(-2.0f);
        }
        if (key == GLFW_KEY_RIGHT)
        {
            character->move_x(2.0f);
        }
        
    
    
}

void Window::mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
    if (action == GLFW_PRESS)
    {
        // Check if mouse left button is pressed, enter rotation mode
        if (button == GLFW_MOUSE_BUTTON_LEFT) {
            rot = true;
            glfwGetCursorPos(window, &prev_x, &prev_y);
        }
    } else if (action == GLFW_RELEASE) {
        // Check if mouse left button is released, exit rotation mode
        if (button == GLFW_MOUSE_BUTTON_LEFT) {
            rot = false;
        }
    }
}

void Window::cursor_pos_callback(GLFWwindow* window, double xpos, double ypos)
{
    curr_x = xpos;
    curr_y = ypos;
    
    if (rot) {
        lastPoint = trackballMapping(prev_x, prev_y);
        curPoint = trackballMapping(curr_x, curr_y);
        
        if (lastPoint == curPoint) return;
        
        float velocity = length(curPoint-lastPoint);
        if (velocity < 0.0001) return;
        
        rotAngle = velocity;
        rotAxis = glm::cross(lastPoint, curPoint);
        
        // extract 3x3 rotation mtx, apply to VIEW
        cam_pos = glm::mat3(glm::rotate(glm::mat4(1.0f), rotAngle*50/180 * glm::pi<float>(), rotAxis)) * cam_pos;
        V = glm::lookAt(cam_pos, cam_look_at, cam_up);
    }
    // Keep track of previous position
    prev_x = curr_x;
    prev_y = curr_y;
}

void Window::scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    V = glm::translate(glm::mat4(1.0f), glm::vec3(0, 0, -1 * yoffset)) * V;
    // extract current camera position from VIEW, update cam_pos
    cam_pos = getCamPos(V);
}

glm::vec3 Window::trackballMapping(double xpos, double ypos) {
    glm::vec3 v;
    float d;
    
    v.x = (2.0 * xpos - Window::width)/Window::width;
    v.y = (Window::height - 2.0 * ypos)/Window::height;
    v.z = 0.0;
    d = glm::length(v);
    d = (d < 1) ? d : 1.0;
    v.z = sqrtf(1.0001 - d*d);
    v = glm::normalize(v);
    return v;
}

// Extracting camera position from a ModelView Matrix
glm::vec3 Window::getCamPos(glm::mat4 modelview)
{
    glm::mat3 rotMat(modelview);
    glm::vec3 d(modelview[3]);
    
    glm::vec3 retVec = -d * rotMat;
    return retVec;
}

// Checking collision between AABB and AABB
bool Window::checkCollision(BoundingBox* a, BoundingBox* b)
{
    return (a->min_x <= b->max_x && a->max_x >= b->min_x) &&
    (a->min_y <= b->max_y && a->max_y >= b->min_y) &&
    (a->min_z <= b->max_z && a->max_z >= b->min_z);
}

glm::vec3 Window::getCurrentCameraPos() {
    return cam_pos;
}
