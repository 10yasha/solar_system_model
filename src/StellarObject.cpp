#include "StellarObject.h"

#define EARTH_RADIUS 6371 // in kilometers

std::vector<StellarObject> initStellarObjects(std::vector<std::unique_ptr<Mesh>>& meshes)
{
	std::vector<StellarObject> stellarObjects;

	// axial tilt values according to https://en.wikipedia.org/wiki/Axial_tilt
	// length of year according to https://spaceplace.nasa.gov/years-on-other-planets/en/
	// size of moons according to https://www.worldatlas.com/articles/biggest-moons-in-our-solar-system.html
	std::tuple<std::string, std::string, double, double, double, double, double> stellarObjectInfos[15] =
	/*{ // to-scale values
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
		std::make_tuple("sun", "", 7.25, 14.18, 696340, 0, 0.f),
		std::make_tuple("mercury", "sun", 0.03, 6.14, 2440, 3, 0.f),
		std::make_tuple("venus", "sun", 2.64, -1.48, 6052, 5, 0.f),
		std::make_tuple("earth", "sun", 23.44, 360.99, 6371, 6, 0.f),
		std::make_tuple("mars", "sun", 25.19, 350.89, 3390, 7, 0.f),
		std::make_tuple("jupiter", "sun", 3.13, 870.54, 69911, 9, 0.f),
		std::make_tuple("saturn", "sun", 26.73, 810.79, 58232, 11, 0.f),
		std::make_tuple("uranus", "sun", 82.23, -501.16, 25362, 13, 0.f),
		std::make_tuple("neptune", "sun", 28.32, 536.31, 24622, 15, 0.f),
		std::make_tuple("moon", "earth", 0, 30.f, 1737.5, 1.5, 140.f),
		std::make_tuple("titan", "saturn", 0, 30.f, 2575., 1.5, 140.f),
		std::make_tuple("io", "jupiter", 0, 30.f, 1821.5, 1., 60.f),
		std::make_tuple("europa", "jupiter", 0, 30.f, 1561., 2., 80.f),
		std::make_tuple("ganymede", "jupiter", 0, 30.f, 2631., 3., 100.f),
		std::make_tuple("callisto", "jupiter", 0, 30.f, 2410.5, 5., 120.f),
	};

	// a and b values for parametric equation of an ellipse given here
	// http://www.ijsrp.org/research-paper-0516/ijsrp-p5328.pdf
	std::tuple<double, double> ellipseParams[15] =
	{
		std::make_tuple(0.0f, 0.0f),
		std::make_tuple(320.0f, 320.0f),
		std::make_tuple(335.0f, 335.0f),
		std::make_tuple(350.0f, 350.0f),
		std::make_tuple(365.0f, 365.0f),
		std::make_tuple(500.0f, 500.0f),
		std::make_tuple(600.0f, 600.0f),
		std::make_tuple(680.0f, 680.0f),
		std::make_tuple(720.0f, 720.0f),
		std::make_tuple(5.0f, 5.0f),
		std::make_tuple(40.0f, 40.0f),
		std::make_tuple(40.0f, 40.0f),
		std::make_tuple(45.0f, 45.0f),
		std::make_tuple(50.0f, 50.0f),
		std::make_tuple(55.0f, 55.0f)
	};
	/*{ // to-scale values
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
	ASSERT(sizeof(ellipseParams) / sizeof(ellipseParams[0]) == meshes.size());

	for (unsigned int i = 0; i < meshes.size(); i++)
	{
		stellarObjects.push_back(StellarObject(
			std::get<0>(stellarObjectInfos[i]), // name
			std::get<1>(stellarObjectInfos[i]), // orbital focus
			std::get<2>(stellarObjectInfos[i]), // axial tilt
			std::get<3>(stellarObjectInfos[i]), // rotation speed
			std::get<4>(stellarObjectInfos[i]), // object radius
			std::get<5>(stellarObjectInfos[i]), // length of year in days
			std::get<6>(stellarObjectInfos[i]), // starting angle
			std::get<0>(ellipseParams[i]), // ellipse param a
			std::get<1>(ellipseParams[i]), // ellipse param b
			std::move(meshes[i]) // vectices/indices to draw
		));
	}

	// sets ptr from each object to object it orbits around
	for (auto& stellarObject : stellarObjects)
	{
		stellarObject.m_orbitalFocusPtr = getPtrToStellarObject(stellarObjects, stellarObject.m_orbitalFocus);
	}

	return stellarObjects;
}

StellarObject* getPtrToStellarObject(std::vector<StellarObject>& stellarObjects, std::string orbitalFocus)
{
	if (orbitalFocus == "") // in case of the sun, which does not rotate around anything
	{
		return nullptr;
	}

	for (auto& stellarObject : stellarObjects)
	{
		if (orbitalFocus == stellarObject.m_name)
		{
			return &stellarObject;
		}
	}

	return nullptr;
}

StellarObject::StellarObject(std::string name, std::string orbitalFocus,
	double axialTilt, double rotationSpeed, double objectRadius,
	double lengthOfYear, double startingAngle, double a, double b, std::unique_ptr<Mesh> mesh)
	: m_name(name), m_orbitalFocus(orbitalFocus),
	m_axialTilt(axialTilt), m_rotationSpeed(rotationSpeed), m_objectRadius(objectRadius),
	m_lengthOfYear(lengthOfYear), m_a(a), m_b(b), m_orbitalFocusPtr(nullptr), m_mesh(std::move(mesh))
{
	m_curObjectRotation = 0.f; // default value
	m_curOrbitalRotation = startingAngle; // default value

	// starting position
	float x = a * std::sin(PI * 2 * m_curOrbitalRotation / 360);
	float y = b * std::cos(PI * 2 * m_curOrbitalRotation / 360);
	m_locMat = glm::translate(glm::mat4(1), glm::vec3(x, 0.0f, y));

	m_orbitalEllipse = std::make_unique<OrbitalEllipse>(a, b);
}

StellarObject::~StellarObject()
{
}

StellarObject::StellarObject(StellarObject&& other) noexcept
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

	m_orbitalFocus = other.m_orbitalFocus;
	m_orbitalFocusPtr = other.m_orbitalFocusPtr;

	m_mesh = std::move(other.m_mesh);
	m_orbitalEllipse = std::move(other.m_orbitalEllipse);
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

	// TRANSLATION 1 //
	float x = m_a * std::sin(PI * 2 * m_curOrbitalRotation);
	float y = m_b * std::cos(PI * 2 * m_curOrbitalRotation);
	m_locMat = glm::translate(glm::mat4(1), glm::vec3(x, 0.0f, y));
}

void StellarObject::exportToShader(Shader& shader, const char* uniform)
{
	shader.bind();

	// ROTATION //
	glm::mat4 matRotation = glm::mat4(1.0f);

	// axial tilt rotation, m_axialTilt will be constant for the stellar object
	matRotation = glm::rotate(matRotation, glm::radians(float(m_axialTilt)), glm::vec3(0.f, 0.f, 1.f));

	// current rotation angle
	matRotation = glm::rotate(matRotation, glm::radians(float(m_curObjectRotation)), glm::vec3(0.f, 1.f, 0.f));

	// SCALE //
	glm::mat4 matScale = glm::mat4(1.0f);

	float scaleFactor = float(m_objectRadius) / EARTH_RADIUS;
	matScale = glm::scale(matScale, glm::vec3(scaleFactor, scaleFactor, scaleFactor));
	
	// TRANSLATION 2 //
	// due to object it orbits around
	glm::mat4 orbitalFocusMat;
	if (m_orbitalFocusPtr == nullptr)
	{
		orbitalFocusMat = glm::mat4(1.0f);
	}
	else
	{
		orbitalFocusMat = m_orbitalFocusPtr->m_locMat;
	}

	// exports the model matrix to the vertex shader
	glUniformMatrix4fv(glGetUniformLocation(shader.m_ID, uniform), 1, GL_FALSE, 
		glm::value_ptr(orbitalFocusMat * m_locMat * matRotation * matScale));
}

void StellarObject::draw(Shader& shader)
{
	m_mesh->draw(shader);
}
