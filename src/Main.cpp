#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stb/stb_image.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>
//#include <filesystem>

// change to std::filesystem depending on your compiler
//namespace fs = std::experimental::filesystem;

#include "Mesh.h"
#include "Shader.h"
#include "LoadModel.h"
#include "Camera.h"
#include "Skybox.h"
#include "GLErrors.h"

// window size
#define WIDTH 1500
#define HEIGHT 800

void scrollCallback(GLFWwindow* window, double xoffset, double yoffset);

// must create as global, due to having to use callback for scroll wheel which only takes function pointer
// (as oppposed to class function pointer)
// it was between a global or a singleton, they're both bad... I guess I'd rather a global than a singleton xD
Camera camera(WIDTH, HEIGHT, 45.f, 0.1f, 1000.f, glm::vec3(0.0f, 0.0f, 10.0f), glm::vec3(0.0f, 0.0f, -1.0f));

int main()
{
	glfwInit();

	// Using OpenGL 3.3
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);

	// CORE profile -> only have the modern functions
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// setup window
	GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "Solar System Model", nullptr, nullptr);
	
	if (window == nullptr) // In case fails to create
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);

	//Load GLAD (loads functions from GPU provided by manufacturer, intel for me)
	gladLoadGL();

	// screen size within window
	glViewport(0, 0, WIDTH, HEIGHT);

	// load shaders and link into one program
	GLCall(Shader defaultShader("./src/shaders/default.vert", "./src/shaders/default.frag"));
	GLCall(Shader skyboxShader("./src/shaders/skybox.vert", "./src/shaders/skybox.frag"));
	
	// planets
	std::vector<std::unique_ptr<Mesh>> meshes;

	// loads planet models
	GLCall(loadModel("./resources/models/earth.obj", meshes));

	// planet model matrix
	glm::mat4 model;

	// variables to define rotation
	float rotation = 0.f;
	double prevTime = glfwGetTime();
	double curTime;
	
	// skybox
	Skybox skybox("./resources/milky_way_skybox/");

	// set scrollCallback for the camera
	glfwSetScrollCallback(window, scrollCallback);

	// necessary so depth in 3D models rendered properly
	glEnable(GL_DEPTH_TEST);

	// main loop here
	while (!glfwWindowShouldClose(window))
	{
		// background
		//glClearColor(0.07f, 0.13f, 0.17f, 1.0f); // navy blue ish
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f); // black
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		defaultShader.bind();

		// simple timer to do rotation
		curTime = glfwGetTime();
		if (curTime - prevTime >= 1 / 60)
		{
			double timDiff = curTime - prevTime;
			rotation += 0.105f;
			prevTime = curTime;
		}

		// need to initilialize matrices
		model = glm::mat4(1.0f);
		/*view = glm::mat4(1.0f);*/

		// calculate mvp to use
		model = glm::rotate(model, glm::radians(rotation), glm::vec3(0.f, 1.f, 0.f));
		/*view = glm::translate(view, glm::vec3(0.0f, 0.0f, -10.0f));
		proj = glm::perspective(glm::radians(45.0f), (float)WIDTH / HEIGHT, 0.1f, 100.f);*/

		// send MVP matrices to the vertex shader
		int modelLoc = glGetUniformLocation(defaultShader.m_ID, "model");
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

		// Handles camera inputs
		camera.getInputs(window);
		// Updates and exports the camera matrix to the Vertex Shader
		camera.exportToShader(defaultShader, "camMatrix");

		// draw single planet currently
		GLCall(meshes[0]->draw(defaultShader));

		// draw the skybox
		skybox.draw(skyboxShader, camera);

		// there are 2 image buffers, one displayed, and one drawn to.
		// must swap buffers in order to display
		glfwSwapBuffers(window);

		// take care of all GLFW events
		glfwPollEvents();
	}

	// clean up
	glfwDestroyWindow(window);
	glfwTerminate();
	return 0;
}


// scroll callback for zooming in and out, cannot be part of class since must be function pointer
void scrollCallback(GLFWwindow* window, double xoffset, double yoffset)
{
	camera.zoom(yoffset);
}