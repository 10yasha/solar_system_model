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
	glm::mat4 view = glm::mat4(1.0f);
	glm::mat4 projection = glm::mat4(1.0f);

	// Makes camera look in the right direction from the right position
	view = glm::lookAt(m_position, m_position + m_orientation, m_upDirection);
	// Adds perspective to the scene
	projection = glm::perspective(glm::radians(m_FOVdeg), (float)m_width / m_height, m_nearPlane, m_farPlane);
	//projection = glm::ortho(-float(m_width)/2, float(m_width)/2, -float(m_height)/2, float(m_height)/2, m_nearPlane, m_farPlane);

	// Exports the camera matrix to the Vertex Shader
	glUniformMatrix4fv(glGetUniformLocation(shader.m_ID, uniform), 1, GL_FALSE, glm::value_ptr(projection * view));
	//std::cout << m_position.x << " " << m_position.y << " " << m_position.z << " " << 
	//	m_orientation.x << " " << m_orientation.y << " " << m_orientation.z << " " << std::endl;
}


void Camera::getInputs(GLFWwindow* window)
{
	// Handles key inputs
	if ((glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) || (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS))
	{
		//m_position += m_speed * m_orientation;
		//m_position += m_speed * m_upDirection;
		m_position += m_speed * movementMag * glm::normalize(glm::cross(glm::cross(m_orientation, m_upDirection), m_orientation));
	}
	if ((glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) || (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS))
	{
		m_position += m_speed * movementMag * -glm::normalize(glm::cross(m_orientation, m_upDirection));
	}
	if ((glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) || (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS))
	{
		//m_position += m_speed * -m_orientation;
		//m_position += m_speed * -m_upDirection;
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


	// Handles mouse inputs
	if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS)
	{
		// Hides mouse cursor
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);

		// Prevents camera from jumping on the first click
		if (m_firstClick)
		{
			glfwSetCursorPos(window, (m_width / 2), (m_height / 2));
			m_firstClick = false;
		}

		// Stores the coordinates of the cursor
		double mouseX;
		double mouseY;
		// Fetches the coordinates of the cursor
		glfwGetCursorPos(window, &mouseX, &mouseY);

		// Normalizes and shifts the coordinates of the cursor such that they begin in the middle of the screen
		// and then "transforms" them into degrees 
		float rotX = m_sensitivity * (float)(mouseY - (m_height / 2)) / m_height;
		float rotY = m_sensitivity * (float)(mouseX - (m_width / 2)) / m_width;

		// Calculates upcoming vertical change in the Orientation
		glm::vec3 newOrientation = glm::rotate(m_orientation, glm::radians(rotX), 
			glm::normalize(glm::cross(m_orientation, m_upDirection)));

		// limit range of orientation angle from up vector to +/- 85 degrees
		if (abs(glm::angle(newOrientation, m_upDirection) - glm::radians(90.0f)) <= glm::radians(85.0f))
		{
			m_orientation = newOrientation;
		}

		// Rotates the Orientation left and right
		m_orientation = glm::rotate(m_orientation, glm::radians(rotY), m_upDirection);

		// Sets mouse cursor to the middle of the screen so that it doesn't end up roaming around
		glfwSetCursorPos(window, (m_width / 2), (m_height / 2));
	}
	else if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_RELEASE)
	{
		// Unhides cursor since camera is not looking around anymore
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);

		// reset to ensure stable camera on next left click
		m_firstClick = true;
	}
}
