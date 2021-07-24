#include "StellarObject.h"

#define EARTH_RADIUS 6371 // in kilometers

std::vector<StellarObject> initStellarObjects(std::vector<std::unique_ptr<Mesh>>& meshes)
{
	std::vector<StellarObject> stellarObjects;

	// axial tilt values according to https://en.wikipedia.org/wiki/Axial_tilt
	std::tuple<std::string, double, double, double, glm::vec3> stellarObjectInfos[9] =
	{
		std::make_tuple("sun", 7.25, 14.18, 696340, glm::vec3(0.0f, 0.0f, 0.0f)),
		std::make_tuple("mercury", 0.03, 6.14, 2440, glm::vec3(500.0f, 0.0f, 0.0f)),
		std::make_tuple("venus", 2.64, -1.48, 6052, glm::vec3(510.0f, 0.0f, 0.0f)),
		std::make_tuple("earth", 23.44, 360.99, 6371, glm::vec3(520.0f, 0.0f, 0.0f)),
		std::make_tuple("mars", 25.19, 350.89, 3390, glm::vec3(530.0f, 0.0f, 0.0f)),
		std::make_tuple("jupiter", 3.13, 870.54, 69911, glm::vec3(580.0f, 0.0f, 0.0f)),
		std::make_tuple("saturn", 26.73, 810.79, 58232, glm::vec3(660.0f, 0.0f, 0.0f)),
		std::make_tuple("uranus", 82.23, -501.16, 25362, glm::vec3(720.0f, 0.0f, 0.0f)),
		std::make_tuple("neptune", 28.32, 536.31, 24622, glm::vec3(760.0f, 0.0f, 0.0f))
	};

	ASSERT(sizeof(stellarObjectInfos)/sizeof(stellarObjectInfos[0]) == meshes.size());

	for (unsigned int i = 0; i < meshes.size(); i++)
	{
		stellarObjects.push_back(StellarObject(
			std::get<0>(stellarObjectInfos[i]), // name
			std::get<1>(stellarObjectInfos[i]), // axialTilt
			std::get<2>(stellarObjectInfos[i]), // rotationSpeed
			std::get<3>(stellarObjectInfos[i]), // radius
			std::get<4>(stellarObjectInfos[i]), // startLocation
			std::move(meshes[i])
		));
	}

	return stellarObjects;
}

StellarObject::StellarObject(std::string name, double axialTilt, double rotationSpeed, double radius,
	glm::vec3 startLocation, std::unique_ptr<Mesh> mesh)
	: m_name(name), m_axialTilt(axialTilt), m_rotationSpeed(rotationSpeed), m_radius(radius),
	m_position(startLocation), m_mesh(std::move(mesh))
{
	m_curRotation = 0.f; // default value
	m_locMat = glm::translate(glm::mat4(1.0f), startLocation);
}

StellarObject::~StellarObject()
{
}

void StellarObject::updateModel(double timeElapsed)
{
	updateRotation(timeElapsed);
	updatePosition(timeElapsed);
}


void StellarObject::updateRotation(double timeElapsed)
{
	m_curRotation += timeElapsed * m_rotationSpeed;
}

void StellarObject::updatePosition(double timeElapsed)
{
	return;
}

void StellarObject::exportToShader(Shader& shader, const char* uniform)
{
	// ROTATION //
	glm::mat4 matRotation = glm::mat4(1.0f);

	// axial tilt rotation, m_axialTilt will be constant for the stellar object
	matRotation = glm::rotate(matRotation, glm::radians(float(m_axialTilt)), glm::vec3(0.f, 0.f, 1.f));

	// current rotation, m_rotation is constantly being updated
	matRotation = glm::rotate(matRotation, glm::radians(float(m_curRotation)), glm::vec3(0.f, 1.f, 0.f));


	// SCALE //
	glm::mat4 matScale = glm::mat4(1.0f);

	float scaleFactor = float(m_radius) / EARTH_RADIUS;
	matScale = glm::scale(matScale, glm::vec3(scaleFactor, scaleFactor, scaleFactor));

	// exports the model matrix to the vertex shader
	glUniformMatrix4fv(glGetUniformLocation(shader.m_ID, uniform), 1, GL_FALSE, glm::value_ptr(m_locMat * matRotation * matScale));
}

void StellarObject::draw(Shader& shader)
{
	// just call draw in the mesh object
	m_mesh->draw(shader);
}
