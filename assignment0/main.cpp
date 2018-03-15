#include "main.h"
#include "Model.h"


GLFWwindow* window;

void error_callback(int error, const char* description)
{
	// Print error
	fputs(description, stderr);
}

void setup_callbacks()
{
	// Set the error callback
	glfwSetErrorCallback(error_callback);
	// Set the key callback
	glfwSetKeyCallback(window, Window::key_callback);
	// Set the window resize callback
	glfwSetFramebufferSizeCallback(window, Window::resize_callback);
}

void setup_glew()
{
	// Initialize GLEW. Not needed on OSX systems.
#ifndef __APPLE__
	GLenum err = glewInit();
	if (GLEW_OK != err)
	{
		/* Problem: glewInit failed, something is seriously wrong. */
		fprintf(stderr, "Error: %s\n", glewGetErrorString(err));
		glfwTerminate();
	}
	fprintf(stdout, "Current GLEW version: %s\n", glewGetString(GLEW_VERSION));
#endif
}

void setup_opengl_settings()
{
#ifndef __APPLE__
	// Setup GLEW. Don't do this on OSX systems.
	setup_glew();
#endif
	// Enable depth buffering
	glEnable(GL_DEPTH_TEST);
	// Related to shaders and z value comparisons for the depth buffer
	glDepthFunc(GL_LEQUAL);
	// Set polygon drawing mode to fill front and back of each polygon
	// You can also use the paramter of GL_LINE instead of GL_FILL to see wireframes
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	// Disable backface culling to render both sides of polygons
	glDisable(GL_CULL_FACE);
	// Set clear color
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

}

void print_versions()
{
	// Get info of GPU and supported OpenGL version
	printf("Renderer: %s\n", glGetString(GL_RENDERER));
	printf("OpenGL version supported %s\n", glGetString(GL_VERSION));

	//If the shading language symbol is defined
#ifdef GL_SHADING_LANGUAGE_VERSION
	std::printf("Supported GLSL version is %s.\n", (char *)glGetString(GL_SHADING_LANGUAGE_VERSION));
#endif
}

int main(void)
{
	// Create the GLFW window
	window = Window::create_window(1280, 760);
	// Print OpenGL and GLSL versions
	print_versions();
	// Setup callbacks
	setup_callbacks();
	// Setup OpenGL settings, including lighting, materials, etc.
	setup_opengl_settings();
	// Initialize objects/pointers for rendering
	Window::initialize_objects();
    
//    // configure global opengl state
//    // -----------------------------
//    glEnable(GL_DEPTH_TEST);
    
    // build and compile shaders
    // -------------------------
    glEnable(GL_DEPTH_TEST);

    Shader ourShader("./modelLoading.vs", "./modelLoading.frag");
    
    // load models
    // -----------
    Model ourModel("nanosuit.obj");

	// Loop while GLFW window should stay open
	while (!glfwWindowShouldClose(window))
	{
		// Main render display callback. Rendering of objects is done here.
//        Window::display_callback(window);
		// Idle callback. Updating objects, etc. can be done here.
//        Window::idle_callback();
        
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


        
        // don't forget to enable shader before setting uniforms
        ourShader.use();
        
        // view/projection transformations
        // Now send these values to the shader program
        glUniformMatrix4fv(glGetUniformLocation(ourShader.ID, "projection"), 1, GL_FALSE, &Window::P[0][0]);
        glUniformMatrix4fv(glGetUniformLocation(ourShader.ID, "view"), 1, GL_FALSE, &Window::V[0][0]);


        // render the loaded model
        glm::mat4 model;
        model = glm::translate(model, glm::vec3(0.0f, -4.0f, 0.0f)); // translate it down so it's at the center of the scene
        model = glm::scale(model, glm::vec3(0.6f, 0.6f, 0.6f));    // it's a bit too big for our scene, so scale it down
        glUniformMatrix4fv(glGetUniformLocation(ourShader.ID, "model"), 1, GL_FALSE, &model[0][0]);

        ourModel.Draw(ourShader);
        
        glfwSwapBuffers(window);
        glfwPollEvents();

        
	}

	Window::clean_up();
	// Destroy the window
	glfwDestroyWindow(window);
	// Terminate GLFW
	glfwTerminate();

	exit(EXIT_SUCCESS);
}
