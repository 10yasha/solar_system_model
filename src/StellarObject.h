#pragma once

#include <vector>
#include <string>
#include <tuple>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Mesh.h"
#include "GLErrors.h"

class StellarObject;

// creates all the planet objects with parameters like axial tilt/duration of single rotation
std::vector<StellarObject> initStellarObjects(std::vector<std::unique_ptr<Mesh>>& meshes);

class StellarObject
{
public:
	std::string m_name;
	double m_axialTilt, m_rotationSpeed, m_radius;
	glm::mat4 m_locMat;
	double m_curRotation;
	std::unique_ptr<Mesh> m_mesh;

	StellarObject(std::string name, double axialTilt, double rotationSpeed, double size, glm::mat4 startLocation, std::unique_ptr<Mesh> mesh);
	~StellarObject();

	// quick move constructor in header for now
	StellarObject(StellarObject&& other) noexcept
	{
		m_name = other.m_name;
		m_axialTilt = other.m_axialTilt;
		m_rotationSpeed = other.m_rotationSpeed;
		m_radius = other.m_radius;
		m_locMat = other.m_locMat;
		m_curRotation = other.m_curRotation;
		m_mesh = std::move(other.m_mesh);
	}

	void updateModel(double timeElapsed);
	
	void updateRotation(double timeElapsed);
	void updatePosition(double timeElapsed);

	void exportToShader(Shader& shader, const char* uniform);

	void draw(Shader& shader);
};