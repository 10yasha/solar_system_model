#pragma once

#include <vector>
#include <string>
#include <tuple>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Mesh.h"
#include "GLErrors.h"

#define PI 3.1415926

class StellarObject;

// creates all the planet objects with parameters like axial tilt/duration of single rotation
std::vector<StellarObject> initStellarObjects(std::vector<std::unique_ptr<Mesh>>& meshes);

class StellarObject
{
public:
	std::string m_name;
	double m_axialTilt, m_rotationSpeed, m_objectRadius, m_lengthOfYear;
	double m_a, m_b;
	glm::mat4 m_locMat;
	double m_curObjectRotation;
	double m_curOrbitalRotation;
	std::unique_ptr<Mesh> m_mesh;

	StellarObject(std::string name, double axialTilt, double rotationSpeed, double objectRadius,
		double lengthOfYear, double a, double b, std::unique_ptr<Mesh> mesh);
	~StellarObject();

	// quick move constructor in header for now
	StellarObject(StellarObject&& other) noexcept
	{
		m_name = other.m_name;
		m_axialTilt = other.m_axialTilt;
		m_rotationSpeed = other.m_rotationSpeed;
		m_objectRadius = other.m_objectRadius;
		m_lengthOfYear = other.m_lengthOfYear;
		m_a = other.m_a;
		m_b = other.m_b;
		m_locMat = other.m_locMat;
		m_curObjectRotation = other.m_curObjectRotation;
		m_curOrbitalRotation = other.m_curOrbitalRotation;
		m_mesh = std::move(other.m_mesh);
	}

	void updateModel(double timeElapsed);
	
	void updateRotation(double timeElapsed);
	void updatePosition(double timeElapsed);

	void exportToShader(Shader& shader, const char* uniform);

	void draw(Shader& shader);
};