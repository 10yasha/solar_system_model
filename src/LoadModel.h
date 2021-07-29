#pragma once

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <map>
#include <vector>

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <stb/stb_image.h>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "mesh.h"
#include "GLErrors.h"

// loads sun, planets, and satellites
void loadSolarSystemModels(std::string directory, std::vector<std::unique_ptr<Mesh>>& meshes);

// loads the asteroid model to use for asteroid belt
std::unique_ptr<Mesh> loadAsteroidModel(std::string directory, const int number, std::vector<glm::mat4> instanceMatrix);

// main routine that will load meshes into a vector of unique pointers used to return the models
void loadModel(std::string path, std::vector<std::unique_ptr<Mesh>>& meshes);

// iterates through aiNodes in tree structure of aiScene defined by assimp which is used to load scenes
static void processNode(aiNode* node, const aiScene* scene, std::vector<std::unique_ptr<Mesh>>& meshes);

// processes each of the aiMesh in the nodes and converts them into mesh class defined in Mesh.h
// Note: these two mesh classes are different
static std::unique_ptr<Mesh> processMesh(aiMesh* mesh, const aiScene* scene);

// loads the texture defined by the material
static Texture loadMaterialTexture(aiMaterial* mat, aiTextureType type);

// performs actual loading of the texture file using stb_image
// sets up config and sends texture data to the GPU
static unsigned int TextureFromFile(std::string texturePath);