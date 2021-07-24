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
#include "StellarObject.h"
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
Camera camera(WIDTH, HEIGHT, 45.f, 0.1f, 10000.f, glm::vec3(176.335f, 195.577f, 757.643f),
	glm::vec3(-0.449704, -0.417709f, -0.789486f));


// global parameter for amount of days that will elapse per second of real time
double daysPerSecond = 0.2f;

int main()
{
	glfwInit();

	// Using OpenGL 3.3
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);

	// CORE profile -> only have the modern functions
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "Solar System Model", nullptr, nullptr);
	
	if (window == nullptr)
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
	GLCall(Shader lightSourceShader("./src/shaders/lightSource.vert", "./src/shaders/lightSource.frag"));
	
	// planets
	std::vector<std::unique_ptr<Mesh>> meshes;

	// loads planet models
	loadPlanetModels("./resources/models/", meshes);

	std::vector<StellarObject> celestialBodies = initStellarObjects(meshes);

	// variables to define rotation
	double prevTime = glfwGetTime();
	double curTime;
	
	// skybox
	Skybox skybox("./resources/milky_way_skybox/");

	// set scrollCallback for the camera
	glfwSetScrollCallback(window, scrollCallback);

	// whether objects will move in their orbit the sun
	bool move = true;

	// light information
	glm::vec3 lightPosition(0.0f, 0.0f, 0.0f);
	glm::vec4 lightColor(1.0f, 1.0f, 1.0f, 1.0f);

	// necessary so depth in 3D models rendered properly
	glEnable(GL_DEPTH_TEST);

	// main loop here
	while (!glfwWindowShouldClose(window))
	{
		// background
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f); // black
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// simple timer to do rotation
		curTime = glfwGetTime();
		if (curTime - prevTime >= 1 / 60)
		{
			double realTimeElapsed = curTime - prevTime;
			prevTime = curTime;

			// update each of the stellar object models for movement that occurred
			for (auto& celestialBody : celestialBodies)
			{
				celestialBody.updateRotation(realTimeElapsed * daysPerSecond);

				if (celestialBody.m_name != "sun")
				{
					if (move)
					{
						celestialBody.updatePosition(realTimeElapsed * daysPerSecond);
					}
				}
			}
		}


		// Handles camera inputs
		camera.getInputs(window);
		// Updates and exports the camera matrix to the Vertex Shader
		camera.exportToShader(defaultShader, "camMatrix");
		camera.exportToShader(lightSourceShader, "camMatrix");

		// draw the sun and all the planets
		for (auto& celestialBody : celestialBodies)
		{
			if (celestialBody.m_name == "sun")
			{
				// export uniform for update model of the object to the GPU
				celestialBody.exportToShader(lightSourceShader, "model");

				// glUniform4f(glGetUniformLocation(lightSourceShader.m_ID, "lightColor"),
				//	lightColor.x, lightColor.y, lightColor.z, lightColor.w);

				// finally draw all the objects
				celestialBody.draw(lightSourceShader);
			}
			else
			{
				// export uniform for update model of the object to the GPU
				celestialBody.exportToShader(defaultShader, "model");

				// glUniform4f(glGetUniformLocation(lightSourceShader.m_ID, "lightColor"),
				//	lightColor.x, lightColor.y, lightColor.z, lightColor.w);
				glUniform3f(glGetUniformLocation(defaultShader.m_ID, "lightColor"),
					lightColor.x, lightColor.y, lightColor.z);
				glUniform3f(glGetUniformLocation(defaultShader.m_ID, "lightPosition"),
					lightPosition.x, lightPosition.y, lightPosition.z);

				// finally draw all the objects
				celestialBody.draw(defaultShader);
			}
		}

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