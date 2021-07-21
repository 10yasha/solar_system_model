#include "Skybox.h"


// constructor
Skybox::Skybox(std::string directory)
{
	initSkybox();
	getTexturePaths(directory);
	createCubeMap();
}

// initializes and send vertices and indices data to buffers
void Skybox::initSkybox() {
	// create VAO, VBO, EBO for the skybox
	glGenVertexArrays(1, &m_vao);
	glGenBuffers(1, &m_vbo);
	glGenBuffers(1, &m_ebo);

	// bind vbo to this vao
	glBindVertexArray(m_vao);

	// send the vertices data
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(m_vertices), &m_vertices, GL_STATIC_DRAW);

	// send the indices data
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(m_indices), &m_indices, GL_STATIC_DRAW);

	// layout
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	// unbind
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

// fills in array of filepaths
void Skybox::getTexturePaths(std::string directory)
{
	// faces of the cubemap
	std::string cubemapPaths[6] =
	/*{
		directory + "right.png",
		directory + "left.png",
		directory + "top.png",
		directory + "bottom.png",
		directory + "front.png",
		directory + "back.png"
	};*/
	/*{
	directory + "right.png",
	directory + "left.png",
	directory + "top.png",
	directory + "bottom.png",
	directory + "back.png",
	directory + "front.png"
	};*/
	/*{
		directory + "GalaxyTex_PositiveX.tga",
		directory + "GalaxyTex_NegativeX.tga",
		directory + "GalaxyTex_PositiveY.tga",
		directory + "GalaxyTex_NegativeY.tga",
		directory + "GalaxyTex_PositiveZ.tga",
		directory + "GalaxyTex_NegativeZ.tga"
	};*/
	{
		directory + "right.png",
		directory + "left.png",
		directory + "top.png",
		directory + "bottom.png",
		directory + "back.png",
		directory + "front.png"
	};

	// copy the data in
	std::copy(cubemapPaths, cubemapPaths + 6, m_cubemapPaths);
}

// uses skybox textures to create cubemap
void Skybox::createCubeMap() {
	// create cubemap texture object
	glGenTextures(1, &m_cubemapTexID);
	glBindTexture(GL_TEXTURE_CUBE_MAP, m_cubemapTexID);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	// prevent seams
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	// This might help with seams on some systems
	//glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);

	// Cycles through all the textures and attaches them to the cubemap object
	for (unsigned int i = 0; i < 6; i++)
	{
		int width, height, nrChannels;
		unsigned char* data = stbi_load(m_cubemapPaths[i].c_str(), &width, &height, &nrChannels, 0);
		if (data)
		{
			stbi_set_flip_vertically_on_load(false);
			//stbi_set_flip_vertically_on_load(true);
			glTexImage2D
			(
				GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
				0,
				GL_RGB,
				width,
				height,
				0,
				GL_RGB,
				GL_UNSIGNED_BYTE,
				data
			);
			stbi_image_free(data);
		}
		else
		{
			std::cout << "Failed to load texture: " << m_cubemapPaths[i] << std::endl;
			stbi_image_free(data);
		}
	}
}

void Skybox::draw(Shader& shader, const Camera& camera)
{
	// Since the cubemap will always have a depth of 1.0, we need that equal sign so it doesn't get discarded
	glDepthFunc(GL_LEQUAL);

	shader.bind();
	glm::mat4 view = glm::mat4(1.0f);
	glm::mat4 projection = glm::mat4(1.0f);
	// We make the mat4 into a mat3 and then a mat4 again in order to get rid of the last row and column
	// The last row and column affect the translation of the skybox (which we don't want to affect)
	view = glm::mat4(glm::mat3(glm::lookAt(camera.m_position, camera.m_position + camera.m_orientation, camera.m_upDirection)));
	projection = glm::perspective(camera.m_FOVdeg, (float)camera.m_width / camera.m_height, camera.m_nearPlane, camera.m_farPlane);
	glUniformMatrix4fv(glGetUniformLocation(shader.m_ID, "camMatrix"), 1, GL_FALSE, glm::value_ptr(projection * view));

	// Draws the cubemap as the last object so we can save a bit of performance by discarding all fragments
	// where an object is present (a depth of 1.0f will always fail against any object's depth value)
	glBindVertexArray(m_vao);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, m_cubemapTexID);
	glDrawElements(GL_TRIANGLES, sizeof(m_indices), GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);

	// Switch back to the normal depth function
	glDepthFunc(GL_LESS);
}