#include <filesystem>
#include <map>
#include <string>

#include "ResourceManager.h"
#include "Texture.h"

#include "stb_image.h"

#include "BoundingBox.h"
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>

ResourceManager RM;

void ResourceManager::LoadAllResources()
{
    for (const auto& entry : std::filesystem::recursive_directory_iterator(ASSETS_PATH)) {
        auto path = entry.path();
        if (entry.is_regular_file()) {
            auto ext = path.extension();

            if (ext == L".png" || ext == L".jpg") {
                auto resource_key = path.filename().replace_extension().string();
                LoadTexture(entry.path().string(), resource_key, ext == L".png");
            }
        }
    }
}

Texture* ResourceManager::GetTexture(std::string resource_key)
{
    if (!loaded_textures.contains(resource_key))
        return nullptr;

    return &loaded_textures[resource_key];
}

Shader* ResourceManager::GetShader(std::string shader_name)
{
    return &loaded_shaders[shader_name];
}

Texture* ResourceManager::LoadTexture(std::string file_path, std::string resource_key, bool alpha)
{
    if (file_path.find(ASSETS_PATH) == std::string::npos) {
        file_path = ASSETS_PATH + file_path;
    }

    Texture texture;
    if (alpha) {
        texture.Internal_Format = GL_RGBA;
        texture.Image_Format = GL_RGBA;
    }

    int width = 0, height = 0, nrChannels = 0;

    unsigned char* data;

    int desiredChannels = 4;

    if (file_path.find("Units") != std::string::npos)
        int desiredChannels = 4;

    data = stbi_load(file_path.c_str(), &width, &height, &nrChannels, desiredChannels);

    if (data == nullptr) {
        std::cout << "ERROR::LoadTexture: Failed to loaded texture: " + file_path << std::endl;
        abort();
    }

    texture.Generate(width, height, data);

    stbi_image_free(data);

    if (loaded_textures.contains(file_path)) {
        std::cout << "ERROR::LoadTexture: Failed to save loaded texture, resource with such name already exists" << std::endl;
        abort();
    }

    std::cout << "ResourceManager::LoadTexture -> Loaded texture: " + file_path << std::endl;

    loaded_textures.insert(std::pair(resource_key, texture));

    return &loaded_textures[resource_key];
}

unsigned int ResourceManager::TextureFromFile(const char* path, const string& directory, bool gamma)
{
    string filename = string(path);
    filename = directory + '/' + filename;

    unsigned int textureID;
    glGenTextures(1, &textureID);

    int width, height, nrComponents;
    unsigned char* data = stbi_load(filename.c_str(), &width, &height, &nrComponents, 0);
    if (data) {
        GLenum format;
        if (nrComponents == 1)
            format = GL_RED;
        else if (nrComponents == 3)
            format = GL_RGB;
        else if (nrComponents == 4)
            format = GL_RGBA;

        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        stbi_image_free(data);
    } else {
        std::cout << "Texture failed to load at path: " << path << std::endl;
        stbi_image_free(data);
    }

    return textureID;
}

Texture* ResourceManager::GetTextureByKey(std::string key)
{
    if (!loaded_textures.contains(key)) {
        return nullptr;
    }
    return &loaded_textures.at(key);
}

vector<std::string> ResourceManager::LoadMaterialTextures(aiMaterial* mat, aiTextureType type, const string typeName, const string& directory)
{
    auto number_of_textures = mat->GetTextureCount(type);
    vector<std::string> keys;
    for (u32 i = 0; i < number_of_textures; i++) {
        auto key = directory + "/" + typeName;
        if (loaded_textures.contains(key)) {
            keys.push_back(key);
            continue;
        }

        aiString str;
        mat->GetTexture(type, i, &str);

        Texture texture;
        texture.ID = TextureFromFile(str.C_Str(), directory, false);
        texture.type = typeName;
        texture.path = std::string(str.C_Str());

        loaded_textures.insert(std::pair(key, texture));
        keys.push_back(key);
    }

    return keys;
}

Model* ResourceManager::LoadModel(std::string file_path, std::string resource_key, bool gamma)
{
    if (loaded_models.contains(resource_key))
        return &loaded_models[resource_key];

    auto path = ASSETS_PATH + file_path;

    Assimp::Importer importer;
    const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_FlipUVs | aiProcess_CalcTangentSpace);

    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
        printf("ERROR::ASSIMP:: $s\n", importer.GetErrorString());
        assert(false);
        return NULL;
    }

    const string directory = path.substr(0, path.find_last_of('/'));

    Model model;

    vector<Mesh> meshes;
    Models::LoadMeshesFromScene(&meshes, scene->mRootNode, scene, directory);
    loaded_meshes.insert(loaded_meshes.end(), meshes.begin(), meshes.end());

    for (u32 i = loaded_meshes.size() - meshes.size(); i < meshes.size(); i++) {
        Mesh* m_ref = &loaded_meshes.at(i);
        model.meshes.push_back(m_ref);
    }

    loaded_models.insert(std::pair(resource_key, model));
    return &loaded_models[resource_key];
}

Shader* ResourceManager::LoadShader(std::string vs, std::string fs, const std::string resource_key)
{
    std::string vertexCode;
    std::string fragmentCode;

    std::string vs_path = ResourceManager::SHADERS_PATH + vs;
    std::string fs_path = ResourceManager::SHADERS_PATH + fs;

    if (loaded_shaders.contains(resource_key)) {
        std::cout << "ERROR::SHADER: Shader with such resource key already exists" << std::endl;
    }

    Shader shader = *new Shader(vs_path.c_str(), fs_path.c_str(), NULL);

    loaded_shaders.insert(std::pair(resource_key, shader));

    return &loaded_shaders[resource_key];
}
