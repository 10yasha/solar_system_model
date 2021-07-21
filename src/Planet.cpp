#include "Planet.h"

std::vector<Planet> initPlanets()
{
	std::vector<Planet> planets;

	// values according to https://en.wikipedia.org/wiki/Axial_tilt
	/*planets.push_back(Planet("sun", 7.25, 14.18, 696340));
	planets.push_back(Planet("mercury", 0.03, 6.14, 2440));
	planets.push_back(Planet("venus", 2.64, -1.48, 6052));
	planets.push_back(Planet("earth", 23.44, 360.99, 6371));
	planets.push_back(Planet("mars", 25.19, 350.89, 3390));
	planets.push_back(Planet("jupiter", 3.13, 870.54, 69911));
	planets.push_back(Planet("saturn", 26.73, 810.79, 58232));
	planets.push_back(Planet("uranus", 82.23, -501.16, 25362));
	planets.push_back(Planet("neptune", 28.32, 536.31, 24622));*/


	/*planets.push_back(Planet("mercury", 0.03, 6.14, 2440));
	planets.push_back(Planet("earth", 23.44, 360.99, 6371));*/

	glm::mat4 rotation = glm::mat4(1.0f);
	glm::mat4 translation = glm::mat4(1.0f);

	rotation = glm::rotate(rotation, float(glm::radians(3.3)), glm::vec3(0.f, 1.f, 0.f));
	translation = glm::translate(translation, glm::vec3(0.0f, 0.0f, -10.0f));

	return planets;
}

Planet::Planet(std::string name, double axialTilt, double rotationSpeed, double size, glm::mat4 location, std::unique_ptr<Mesh> mesh)
	: m_name(name), m_axialTilt(axialTilt), m_rotationSpeed(rotationSpeed), m_size(size), m_location(location), m_mesh(std::move(mesh))
{

}

Planet::~Planet()
{
}

void Planet::updateModel(double timeElapsed)
{
	updateRotation(timeElapsed);
	updateOrbit(timeElapsed);
}


void Planet::updateRotation(double timeElapsed)
{

}

void Planet::updateOrbit(double timeElapsed)
{
	return;
}