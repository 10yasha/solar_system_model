#include "LoadModel.h"

void loadPlanetModels(std::string directory, std::vector<std::unique_ptr<Mesh>>& meshes)
{
    // loads planet models
    std::string modelPaths[9] =
    {
        directory + "sun.obj",
        directory + "mercury.obj",
        directory + "venus.obj",
        directory + "earth.obj",
        directory + "mars.obj",
        directory + "jupiter.obj",
        directory + "saturn.obj",
        directory + "uranus.obj",
        directory + "neptune.obj"
    };

    for (std::string modelPath : modelPaths)
    {
        loadModel(modelPath, meshes);
    }
}


// main routine that will load meshes into a vector of unique pointers used to return the models
void loadModel(std::string path, std::vector<std::unique_ptr<Mesh>>& meshes)
{
    Assimp::Importer importer;
    const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs);

    // error check
    if (!scene || scene->mFlags == AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
    {
        std::cout << "ERROR::ASSIMP:: " << importer.GetErrorString() << std::endl;
        return;
    }

    // set static variable
    directory = path.substr(0, path.find_last_of('/'));

    // starts recursive function that will iterate through all the nodes in the scene
    processNode(scene->mRootNode, scene, meshes);
}

// iterates through aiNodes in tree structure of aiScene defined by assimp which is used to load scenes
static void processNode(aiNode* node, const aiScene* scene, std::vector<std::unique_ptr<Mesh>>& meshes)
{
    // processes meshes in the node
    for (unsigned int i = 0; i < node->mNumMeshes; i++) {
        aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
        meshes.push_back(std::move(processMesh(mesh, scene)));
    }

    // processes children nodes in tree structure
    for (unsigned int i = 0; i < node->mNumChildren; i++) {
        processNode(node->mChildren[i], scene, meshes);
    }
}

// processes each of the aiMesh in the nodes and converts them into mesh class defined in Mesh.h
// Note: these two mesh classes are different!
static std::unique_ptr<Mesh> processMesh(aiMesh* mesh, const aiScene* scene)
{
    std::vector<Vertex> vertices;
    std::vector<GLuint> indices;
    Texture texture; // there will only be 1 texture (only diffuse map) for simplicity

    // vertex data here
    for (unsigned int i = 0; i < mesh->mNumVertices; i++)
    {
        Vertex vertex;

        vertex.Position.x = mesh->mVertices[i].x;
        vertex.Position.y = mesh->mVertices[i].y;
        vertex.Position.z = mesh->mVertices[i].z;

        vertex.Normal.x = mesh->mNormals[i].x;
        vertex.Normal.y = mesh->mNormals[i].y;
        vertex.Normal.z = mesh->mNormals[i].z;

        if (mesh->mTextureCoords[0]) {
            vertex.TexCoor.x = mesh->mTextureCoords[0][i].x;
            vertex.TexCoor.y = mesh->mTextureCoords[0][i].y;
        }
        else {
            vertex.TexCoor = glm::vec2(0.0f, 0.0f);
        }

        vertices.push_back(vertex);
    }

    // index data here, found within faces
    for (unsigned int i = 0; i < mesh->mNumFaces; i++)
    {
        aiFace face = mesh->mFaces[i];
        for (GLuint j = 0; j < face.mNumIndices; j++)
        {
            indices.push_back(face.mIndices[j]);
        }
    }

    // material data here
    if (mesh->mMaterialIndex >= 0)
    {
        aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
        texture = loadMaterialTexture(material, aiTextureType_DIFFUSE);
    }
    return std::make_unique<Mesh>(vertices, indices, texture);
}

// loads the texture defined by the material
static Texture loadMaterialTexture(aiMaterial* mat, aiTextureType type)
{
    Texture texture;
    aiString textureFilename;

    // gets the name of the texture file defined in '.mat' file
    mat->GetTexture(type, 0, &textureFilename);

    texture.path = std::string(textureFilename.C_Str());

    // gets the ID, loads the texture, sets up config and sends texture to GPU
    texture.ID = TextureFromFile(directory + '/' + texture.path);

    return texture;
}

// performs actual loading of the texture file using stb_image
// sets up config and sends texture data to the GPU
static unsigned int TextureFromFile(std::string texturePath)
{
    int widthImg, heightImg, numCh;
    unsigned char* bytes;
    unsigned int textureID;

    // flips the image so it appears right side up?
    stbi_set_flip_vertically_on_load(false);

    // read image data
    bytes = stbi_load(texturePath.c_str(), &widthImg, &heightImg, &numCh, 0);

    // generates the OpenGL texture object
    glGenTextures(1, &textureID);

    // not good practice to do here, but for this case of only 1 texture, is fine
    glActiveTexture(GL_TEXTURE0);

    // puts texture into openGL format to use, some config
    glBindTexture(GL_TEXTURE_2D, textureID);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, widthImg, heightImg, 0, GL_RGB, GL_UNSIGNED_BYTE, bytes);
    glGenerateMipmap(GL_TEXTURE_2D);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBindTexture(GL_TEXTURE_2D, 0);

    stbi_image_free(bytes);

    return textureID;
}
