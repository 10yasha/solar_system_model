#include"Camera.h"

Camera::Camera(int width, int height, float FOVdeg, float nearPlane, float farPlane,
	glm::vec3 position, glm::vec3 orientation)
	: m_width(width), m_height(height), m_FOVdeg(FOVdeg), m_nearPlane(nearPlane),
	m_farPlane(farPlane), m_position(position), m_orientation(orientation)
{
}

void Camera::zoom(double yoffset)
{
	m_position += float(yoffset) * m_zoomSpeed * m_orientation * movementMag;
}

void Camera::exportToShader(Shader& shader, const char* uniform)
{
	shader.bind();

	// set camera position and direction
	glm::mat4 view = glm::lookAt(m_position, m_position + m_orientation, m_upDirection);
	// use perspective rather than orthographic, 45 degrees is non distorted
	glm::mat4 projection = glm::perspective(glm::radians(m_FOVdeg), (float)m_width / m_height, m_nearPlane, m_farPlane);

	glUniformMatrix4fv(glGetUniformLocation(shader.m_ID, uniform), 1, GL_FALSE, glm::value_ptr(projection * view));

	// to debug current camera position
	//std::cout << m_position.x << " " << m_position.y << " " << m_position.z << " " << 
	//	m_orientation.x << " " << m_orientation.y << " " << m_orientation.z << " " << std::endl;
}


void Camera::getInputs(GLFWwindow* window)
{
	// all key inputs handled
	if ((glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) || (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS))
	{
		m_position += m_speed * movementMag * glm::normalize(glm::cross(glm::cross(m_orientation, m_upDirection), m_orientation));
	}
	if ((glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) || (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS))
	{
		m_position += m_speed * movementMag * -glm::normalize(glm::cross(m_orientation, m_upDirection));
	}
	if ((glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) || (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS))
	{
		m_position += m_speed * movementMag * -glm::normalize(glm::cross(glm::cross(m_orientation, m_upDirection), m_orientation));
	}
	if ((glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) || (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS))
	{
		m_position += m_speed * movementMag * glm::normalize(glm::cross(m_orientation, m_upDirection));
	}
	if (glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS)
	{
		movementMag *= 1.1f;
	}
	if (glfwGetKey(window, GLFW_KEY_O) == GLFW_PRESS)
	{
		movementMag /= 1.1f;
	}
	if (glfwGetKey(window, GLFW_KEY_M) == GLFW_PRESS)
	{
		enableOrbitalMotion = !enableOrbitalMotion;
	}
		


	// mouse inputs to drag screen
	if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS)
	{
		// hides the mouse
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);

		// use a flag to prevent camera from jumping on first click
		if (m_firstClick)
		{
			glfwSetCursorPos(window, (m_width / 2), (m_height / 2));
			m_firstClick = false;
		}

		// cursor coordinates
		double mouseX;
		double mouseY;
		glfwGetCursorPos(window, &mouseX, &mouseY);

		// noormalizes and set mouse coordinates to begin in the middle of the screen
		float rotX = m_sensitivity * (float)(mouseY - (m_height / 2)) / m_height;
		float rotY = m_sensitivity * (float)(mouseX - (m_width / 2)) / m_width;

		// calculates vertical change in the orientation
		glm::vec3 newOrientation = glm::rotate(m_orientation, glm::radians(rotX), 
			glm::normalize(glm::cross(m_orientation, m_upDirection)));

		// limit range of orientation angle in respect to up vector between +/- 85 degrees
		if (abs(glm::angle(newOrientation, m_upDirection) - glm::radians(90.0f)) <= glm::radians(85.0f))
		{
			m_orientation = newOrientation;
		}

		m_orientation = glm::rotate(m_orientation, glm::radians(rotY), m_upDirection);

		// move mouse back to middle of screen
		glfwSetCursorPos(window, (m_width / 2), (m_height / 2));
	}
	else if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_RELEASE)
	{
		// displays mouse again
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);

		// reset first click flag, to ensure stability
		m_firstClick = true;
	}
}