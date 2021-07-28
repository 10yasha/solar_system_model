#include "StellarObject.h"

#define EARTH_RADIUS 6371 // in kilometers

std::vector<StellarObject> initStellarObjects(std::vector<std::unique_ptr<Mesh>>& meshes)
{
	std::vector<StellarObject> stellarObjects;

	// axial tilt values according to https://en.wikipedia.org/wiki/Axial_tilt
	// length of year according to https://spaceplace.nasa.gov/years-on-other-planets/en/
	std::tuple<std::string, double, double, double, double> stellarObjectInfos[9] =
	/*{
		std::make_tuple("sun", 7.25, 14.18, 696340, 0),
		std::make_tuple("mercury", 0.03, 6.14, 2440, 88),
		std::make_tuple("venus", 2.64, -1.48, 6052, 225),
		std::make_tuple("earth", 23.44, 360.99, 6371, 365),
		std::make_tuple("mars", 25.19, 350.89, 3390, 687),
		std::make_tuple("jupiter", 3.13, 870.54, 69911, 4333),
		std::make_tuple("saturn", 26.73, 810.79, 58232, 10759),
		std::make_tuple("uranus", 82.23, -501.16, 25362, 30687),
		std::make_tuple("neptune", 28.32, 536.31, 24622, 60190)
	};*/
	{
		std::make_tuple("sun", 7.25, 14.18, 696340, 0),
		std::make_tuple("mercury", 0.03, 6.14, 2440, 3),
		std::make_tuple("venus", 2.64, -1.48, 6052, 5),
		std::make_tuple("earth", 23.44, 360.99, 6371, 6),
		std::make_tuple("mars", 25.19, 350.89, 3390, 7),
		std::make_tuple("jupiter", 3.13, 870.54, 69911, 9),
		std::make_tuple("saturn", 26.73, 810.79, 58232, 11),
		std::make_tuple("uranus", 82.23, -501.16, 25362, 13),
		std::make_tuple("neptune", 28.32, 536.31, 24622, 15)
	};

	// a and b values for parametric equation of an ellipse given here
	// http://www.ijsrp.org/research-paper-0516/ijsrp-p5328.pdf
	std::tuple<double, double> ellipseParams[9] =
	{
		std::make_tuple(0.0f, 0.0f),
		std::make_tuple(320.0f, 320.0f),
		std::make_tuple(330.0f, 330.0f),
		std::make_tuple(340.0f, 340.0f),
		std::make_tuple(350.0f, 350.0f),
		std::make_tuple(470.0f, 470.0f),
		std::make_tuple(540.0f, 540.0f),
		std::make_tuple(600.0f, 600.0f),
		std::make_tuple(640.0f, 640.0f)
	};
	/*{
		std::make_tuple(0.0f, 0.0f),
		std::make_tuple(57.9f, 56.6703f),
		std::make_tuple(108.f, 107.9974f),
		std::make_tuple(150.f, 149.9783f),
		std::make_tuple(228.f, 226.9905f),
		std::make_tuple(779.f, 778.0643f),
		std::make_tuple(1430.f, 1488.1149f),
		std::make_tuple(2870.f, 2866.9619f),
		std::make_tuple(4500.f, 4499.7277f)
	};*/

	ASSERT(sizeof(stellarObjectInfos)/sizeof(stellarObjectInfos[0]) == meshes.size());

	for (unsigned int i = 0; i < meshes.size(); i++)
	{
		stellarObjects.push_back(StellarObject(
			std::get<0>(stellarObjectInfos[i]), // name
			std::get<1>(stellarObjectInfos[i]), // axialTilt
			std::get<2>(stellarObjectInfos[i]), // rotationSpeed
			std::get<3>(stellarObjectInfos[i]), // object radius
			std::get<4>(stellarObjectInfos[i]), // length of year in days
			std::get<0>(ellipseParams[i]), // ellipse param a
			std::get<1>(ellipseParams[i]), // ellipse param b
			std::move(meshes[i])
		));
	}

	return stellarObjects;
}

StellarObject::StellarObject(std::string name, double axialTilt, double rotationSpeed, double objectRadius,
	double lengthOfYear, double a, double b, std::unique_ptr<Mesh> mesh)
	: m_name(name), m_axialTilt(axialTilt), m_rotationSpeed(rotationSpeed), m_objectRadius(objectRadius),
	m_lengthOfYear(lengthOfYear), m_a(a), m_b(b), m_mesh(std::move(mesh))
{
	m_curObjectRotation = 0.f; // default value
	m_curOrbitalRotation = 0.f; // default value

	// starting position
	float x = a * sin(PI * 2 * m_curOrbitalRotation / 360);
	float y = b * cos(PI * 2 * m_curOrbitalRotation / 360);
	m_locMat = glm::translate(glm::mat4(1), glm::vec3(x, 0.0f, y));
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
	m_curObjectRotation += timeElapsed * m_rotationSpeed;
}

void StellarObject::updatePosition(double timeElapsed)
{
	m_curOrbitalRotation += timeElapsed / m_lengthOfYear;

	// TRANSLATION //
	float x = m_a * sin(PI * 2 * m_curOrbitalRotation);
	float y = m_b * cos(PI * 2 * m_curOrbitalRotation);
	m_locMat = glm::translate(glm::mat4(1), glm::vec3(x, 0.0f, y));
}

void StellarObject::exportToShader(Shader& shader, const char* uniform)
{
	shader.bind();

	// ROTATION //
	glm::mat4 matRotation = glm::mat4(1.0f);

	// axial tilt rotation, m_axialTilt will be constant for the stellar object
	matRotation = glm::rotate(matRotation, glm::radians(float(m_axialTilt)), glm::vec3(0.f, 0.f, 1.f));

	// current rotation, m_rotation is constantly being updated
	matRotation = glm::rotate(matRotation, glm::radians(float(m_curObjectRotation)), glm::vec3(0.f, 1.f, 0.f));

	// SCALE //
	glm::mat4 matScale = glm::mat4(1.0f);

	float scaleFactor = float(m_objectRadius) / EARTH_RADIUS;
	matScale = glm::scale(matScale, glm::vec3(scaleFactor, scaleFactor, scaleFactor));

	// exports the model matrix to the vertex shader
	glUniformMatrix4fv(glGetUniformLocation(shader.m_ID, uniform), 1, GL_FALSE, glm::value_ptr(m_locMat * matRotation * matScale));
}

void StellarObject::draw(Shader& shader)
{
	// just call draw in the mesh object
	m_mesh->draw(shader);
}
