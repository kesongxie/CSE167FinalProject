#ifndef _WINDOW_H_
#define _WINDOW_H_

#include <iostream>

#define GLFW_INCLUDE_GLEXT
#ifdef __APPLE__
#define GLFW_INCLUDE_GLCOREARB
#else
#include <GL/glew.h>
#endif
#include <GLFW/glfw3.h>
#include "shader.h"

#include "BoundingBox.h"

class Window
{
public:
	static int width;
	static int height;
	static glm::mat4 P; // P for projection
	static glm::mat4 V; // V for view
    static bool showExplode;
    static glm::vec3 explodePosition;
	static void initialize_objects();
	static void clean_up();
	static GLFWwindow* create_window(int width, int height);
	static void resize_callback(GLFWwindow* window, int width, int height);
	static void idle_callback();
	static void display_callback(GLFWwindow*);
	static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
    // keyboard controls
    static void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
    static void cursor_pos_callback(GLFWwindow* window, double xpos, double ypos);
    static void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);

    
    
    // trackball mapping
    static glm::vec3 trackballMapping(double, double);
    // get current cam position
    static glm::vec3 getCamPos(glm::mat4);
    // bounding box
    static bool bbox_display;
    static std::vector<BoundingBox*> bbox_vector;
    static bool checkCollision(BoundingBox*, BoundingBox*);
    static glm::vec3 getCurrentCameraPos();
};

#endif
