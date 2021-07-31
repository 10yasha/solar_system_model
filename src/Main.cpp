#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stb/stb_image.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "imgui/imgui.h"
#include "imgui/imgui_impl_opengl3.h"
#include "imgui/imgui_impl_glfw.h"

#include <math.h>
#include <iostream>

#include "Mesh.h"
#include "Shader.h"
#include "LoadModel.h"
#include "StellarObject.h"
#include "Camera.h"
#include "Skybox.h"
#include "GLErrors.h"
#include "GUIParams.h"
#include "OrbitalEllipse.h"

// window size
#define WIDTH 1500
#define HEIGHT 800

// rng to generate number between -1 and 1
float randfloat()
{
	return -1.0f + (rand() / (RAND_MAX / 2.0f));
}

// rng to generate number between -1 to -0.3 or 0.3 to 1
float randscale()
{
	return (0.3f + (rand() / RAND_MAX * 0.7f)) * ((rand() % 2) * 2 - 1);
}

// generates model matrices for random asteroids
std::vector<glm::mat4> genAsteroidModels(const unsigned int numberAsteroids, double radius, double radiusDeviation);

// must create as global, due to having to use callback for scroll wheel which only takes function pointer
// (as oppposed to class function pointer)
// it was between a global or a singleton, they're both bad... I guess I'd rather a global than a singleton xD
Camera camera(WIDTH, HEIGHT, 45.f, 0.1f, 10000.f, glm::vec3(133.887, 84.8933, 759.807),
	glm::vec3(-0.496603, -0.391775, -0.774533));

// globals in GUIParams.h
float daysPerSecond = 0.2f;
bool enableOrbitalMotion = false;
bool enableRotationalMotion = true;
bool enableOrbitalPath = false;
int movementSensitivity = 0;

int main()
{
	glfwInit();

	// modernOpenGL is 3.3+
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "Solar System Model", nullptr, nullptr);
	
	if (window == nullptr)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);

	// GLAD -> loads implementation from GPU provided by manufacturer, ie. Intel, AMD
	gladLoadGL();

	glViewport(0, 0, WIDTH, HEIGHT);

	// light information
	glm::vec3 lightPosition(0.0f, 0.0f, 0.0f);
	glm::vec4 lightColor(1.0f, 1.0f, 1.0f, 1.0f);

	// load and link shaders
	GLCall(Shader defaultShader("./src/shaders/default.vert", "./src/shaders/default.frag")); // planets/satellites
	GLCall(Shader skyboxShader("./src/shaders/skybox.vert", "./src/shaders/skybox.frag")); // background
	GLCall(Shader lightSourceShader("./src/shaders/lightSource.vert", "./src/shaders/lightSource.frag")); // the sun
	GLCall(Shader asteroidShader("./src/shaders/asteroid.vert", "./src/shaders/asteroid.frag")); // asteroid belt
	GLCall(Shader orbitShader("./src/shaders/orbit.vert", "./src/shaders/orbit.frag")); // orbit trajectory
	
	glUniform3f(glGetUniformLocation(asteroidShader.m_ID, "lightColor"),
		lightColor.x, lightColor.y, lightColor.z);
	glUniform3f(glGetUniformLocation(asteroidShader.m_ID, "lightPosition"),
		lightPosition.x, lightPosition.y, lightPosition.z);

	auto orbit = OrbitalEllipse(350.f, 350.f);

	// generated randomized asteroids for asteroid belt, instancing enabled
	const unsigned int numberAsteroids = 500;
	float radius = 400.0f;
	float radiusDeviation = 25.0f;

	auto instanceMatrix = genAsteroidModels(numberAsteroids, radius, radiusDeviation);
	auto asteroid = loadAsteroidModel("./resources/models/", numberAsteroids, instanceMatrix);

	// load sun/planets/satellites
	std::vector<std::unique_ptr<Mesh>> meshes;
	loadSolarSystemModels("./resources/models/", meshes);
	std::vector<StellarObject> stellarObjects = initStellarObjects(meshes);

	// will track real time
	double prevTime = glfwGetTime();
	double curTime;
	
	Skybox skybox("./resources/milky_way_skybox/");

	// enable zooming through scroll on mouse
	// glfwSetScrollCallback(window, scrollCallback);
	glfwSetScrollCallback(
		window,
		[](GLFWwindow* window, double xoffset, double yoffset) { camera.zoom(yoffset); }
		);

	// setup Dear ImGui context
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	// setup Platform/Renderer bindings
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init("#version 330 core");
	ImGui::StyleColorsDark();

	// necessary so depth in 3D models rendered properly
	glEnable(GL_DEPTH_TEST);

	// MAIN LOOP //
	while (!glfwWindowShouldClose(window))
	{
		// background
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// start new frame for imgui
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		// update orbital/rotational positions
		curTime = glfwGetTime();
		if (curTime - prevTime >= 1 / 60)
		{
			double realTimeElapsed = curTime - prevTime;
			prevTime = curTime;
			
			if (enableRotationalMotion)
			{
				for (auto& stellarObject : stellarObjects)
				{
					stellarObject.updateRotation(realTimeElapsed * daysPerSecond);
				}
			}

			if (enableOrbitalMotion)
			{
				for (auto& stellarObject : stellarObjects)
				{
					if (stellarObject.m_name != "sun")
						stellarObject.updatePosition(realTimeElapsed * daysPerSecond);
				}
			}
		}

		// update camera
		camera.getInputs(window);
		camera.exportToShader(defaultShader, "camMatrix");
		camera.exportToShader(lightSourceShader, "camMatrix");
		camera.exportToShader(asteroidShader, "camMatrix");
		camera.exportToShader(orbitShader, "camMatrix");

		// draw the sun, planets, satellites/moons
		for (auto& stellarObject : stellarObjects)
		{
			if (stellarObject.m_name == "sun")
			{
				// export model uniform to GPU, then draw
				stellarObject.exportToShader(lightSourceShader, "model");
				stellarObject.draw(lightSourceShader);
			}
			else
			{
				// export model + light uniforms to GPU, then draw
				stellarObject.exportToShader(defaultShader, "model");

				glUniform3f(glGetUniformLocation(defaultShader.m_ID, "lightColor"),
					lightColor.x, lightColor.y, lightColor.z);
				glUniform3f(glGetUniformLocation(defaultShader.m_ID, "lightPosition"),
					lightPosition.x, lightPosition.y, lightPosition.z);

				stellarObject.draw(defaultShader);

				if (enableOrbitalPath)
				{
					stellarObject.m_orbitalEllipse->draw(orbitShader, stellarObject.m_orbitalFocusPtr->m_locMat);
				}
			}
		}

		asteroid->draw(asteroidShader);

		skybox.draw(skyboxShader, camera);

		// imGUI
		ImGui::Begin("Control");
		ImGui::SliderInt("Movement Sensitivity", &movementSensitivity, -10, +10);
		camera.updateSensitivity(movementSensitivity);
		ImGui::InputFloat("days/second", &daysPerSecond, 0.01f, 5.0f, "%.3f");
		ImGui::Checkbox("Enable Orbital Motion", &enableOrbitalMotion);
		ImGui::Checkbox("Enable Rotational Motion", &enableRotationalMotion);
		ImGui::Checkbox("Enable Orbital Path Marker", &enableOrbitalPath);
		ImGui::Text("Average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
		ImGui::Text("\nControls: WASD/up-left-down-right keys; zoom & drag with mouse");
		ImGui::End();

		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		// there are 2 image buffers, one displayed, and one drawn to
		// must swap buffers in order to display image drawn (to back buffer)
		glfwSwapBuffers(window);

		glfwPollEvents();
	}

	glfwDestroyWindow(window);
	glfwTerminate();
	return 0;
}

std::vector<glm::mat4> genAsteroidModels(const unsigned int numberAsteroids, double radius, double radiusDeviation)
{
	// holds different transformations for each asteroid
	std::vector<glm::mat4> instanceMatrix;

	// additional scaling
	glm::mat4 addScale = glm::scale(glm::mat4(1.0f), glm::vec3(0.5f, 0.5f, 0.5f)); 

	for (unsigned int i = 0; i < numberAsteroids; i++)
	{
		// using equation x^2 + y^2 = radius^2 (circle)
		float x = randfloat(); // -1 to 1
		float finalRadius = radius + randfloat() * radiusDeviation; // 375 to 425
		float y = ((rand() % 2) * 2 - 1) * sqrt(1.0f - x * x); // +/- sqrt(1-x^2) -> -1 to 1

		glm::vec3 tempTranslation;
		glm::quat tempRotation;
		glm::vec3 tempScale;

		// makes the random distribution more even
		if (randfloat() > 0.5f)
		{
			tempTranslation = glm::vec3(y * finalRadius, randfloat(), x * finalRadius);
		}
		else
		{
			tempTranslation = glm::vec3(x * finalRadius, randfloat(), y * finalRadius);
		}
		// random rotations
		tempRotation = glm::quat(1.0f, randfloat(), randfloat(), randfloat());
		// random scales
		tempScale = 0.1f * glm::vec3(randscale(), randscale(), randscale());

		// model matrix components
		glm::mat4 trans = glm::translate(glm::mat4(1.0f), tempTranslation);
		glm::mat4 rot = glm::mat4_cast(tempRotation);
		glm::mat4 sca = glm::scale(glm::mat4(1.0f), tempScale);

		instanceMatrix.push_back(trans * rot * sca * addScale);
	}

	return instanceMatrix;
}