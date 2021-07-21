#pragma once

#include <vector>
#include <string>
#include <glm/glm.hpp>
#include<glm/gtc/matrix_transform.hpp>

#include "Mesh.h"

class Planet;

// creates all the planet objects with parameters like axial tilt/duration of single rotation
std::vector<Planet> initPlanets();

class Planet
{
public:
	std::string m_name;
	double m_axialTilt, m_rotationSpeed, m_size;
	glm::mat4 m_location;
	std::unique_ptr<Mesh> m_mesh;

	Planet(std::string name, double axialTilt, double rotationSpeed, double size, glm::mat4 location, std::unique_ptr<Mesh> mesh);
	~Planet();

	void updateModel(double timeElapsed);
	
	void updateRotation(double timeElapsed);
	void updateOrbit(double timeElapsed);
};