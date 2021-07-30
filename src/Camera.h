#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <glm/gtx/vector_angle.hpp>
#include "imgui/imgui.h"
#include "imgui/imgui_impl_opengl3.h"
#include "imgui/imgui_impl_glfw.h"

#include "Shader.h"
#include "GUIParams.h"

class Camera
{
public:
	// vectors for glm::lookAt() function
	glm::vec3 m_position;
	glm::vec3 m_orientation;
	glm::vec3 m_upDirection = glm::vec3(0.0f, 1.0f, 0.0f);

	// parameters for glm::perspective()
	float m_FOVdeg, m_nearPlane, m_farPlane;

	// dimensions of the window
	int m_width;
	int m_height;

	// constant to control movement of camera
	float m_speed = 0.5f;
	float m_mouseDragScale = 100.f;
	float m_zoomSpeed = 10.f;

	// scaling to interactively control speed and zoom speed
	float m_sensitivity = 1.f;

	// flag to prevent camera from jumping around when first clicking
	bool m_firstClick = true;

	Camera(int width, int height, float FOVdeg, float nearPlane, float farPlane,
		glm::vec3 position, glm::vec3 orientation);

	// zoom based on mouse scroll
	void zoom(double yoffset);

	// updates sensitivity of scroll/wasd movement
	void updateSensitivity(int movementSensitivity);

	// exports the camera matrix to vertex shader
	void exportToShader(Shader& shader, const char* uniform);

	// handles inputs from keyboard and mouse other than mouse scroll
	void getInputs(GLFWwindow* window);
};