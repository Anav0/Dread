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

#include <chrono>
#include <filesystem>
#include <format>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

ResourceManager RM;

using namespace std::filesystem;

void UseTextureShader(Shader* shader, m4 projection)
{
    shader->Use();
    shader->setMat4("projection", projection);
    shader->setBool("hideAlpha", false);
    shader->setInt("imageSampler", 0);
}

void ResourceManager::LoadRequiredResources()
{
    LoadTexture("icons.png", "icons", false, true);

    LoadShader(ShaderType::TexturedQuad, "texture.vert", "texture.frag", "texture");
    LoadShader(ShaderType::Gradient, "gradient.vert", "gradient.frag", "gradient");

    LoadShader(ShaderType::Particles, "particles.vert", "particles.frag", "particles");
    LoadShader(ShaderType::Simple, "simple.vert", "simple.frag", "simple");
    LoadShader(ShaderType::Picking, "picking.vert", "picking.frag", "picking");
    LoadShader(ShaderType::Object, "object.vert", "object.frag", "object");
    LoadShader(ShaderType::Debug, "debug.vert", "debug.frag", "debug");
    LoadShader(ShaderType::Mesh, "mesh.vert", "mesh.frag", "mesh");

    LoadModel("map/map.obj", "map");
    //LoadModel("map/map2.obj", "map2");
}

void ResourceManager::LoadAllResources()
{
    for (const auto& entry : recursive_directory_iterator(ASSETS_PATH)) {
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

Texture* ResourceManager::GetTexture(const std::string& resource_key)
{
    if (!loaded_textures.contains(resource_key))
        return nullptr;

    return &loaded_textures.at(resource_key);
}

Shader* ResourceManager::GetShader(const std::string& shader_name)
{
    return &loaded_shaders.at(shader_name);
}

Texture* ResourceManager::LoadTexture(std::string file_path, const std::string& resource_key, bool absolute_path, bool alpha)
{
    if (!absolute_path && file_path.find(ASSETS_PATH) == std::string::npos) {
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

    return &loaded_textures.at(resource_key);
}

u32 ResourceManager::TextureFromFile(const char* path, const string& directory, bool gamma)
{
    string filename = string(path);
    filename = directory + '/' + filename;

    u32 textureID;
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

Texture* ResourceManager::GetTextureByKey(const std::string& key)
{
    if (!loaded_textures.contains(key)) {
        return nullptr;
    }
    return &loaded_textures.at(key);
}

vector<std::string> ResourceManager::LoadMaterialTextures(aiMaterial* mat, aiTextureType type, const std::string& typeName, const std::string& directory)
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

Model* ResourceManager::GetModel(const std::string& resource_key)
{

    if (loaded_models.contains(resource_key))
        return &loaded_models[resource_key];

    return nullptr;
}

void ResourceManager::LoadModel(const std::string& file_path, const std::string& resource_key, bool gamma)
{
    if (loaded_models.contains(resource_key))
        return;

    auto path = ASSETS_PATH + file_path;

    Assimp::Importer importer;
    const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_FlipUVs | aiProcess_CalcTangentSpace);

    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
        printf("ERROR::ASSIMP:: %s\n", importer.GetErrorString());
        assert(false);
        return;
    }

    const string directory = path.substr(0, path.find_last_of('/'));

    Model model;
    model.name = resource_key;

    Models::LoadMeshesFromScene(model.meshes, scene->mRootNode, scene, directory);

    int i = 0;
    for (auto& mesh : model.meshes) {
        mesh.id = directory + "_" + std::to_string(i);
        i++;
    }

    loaded_models.insert(std::pair(resource_key, model));
    return;
}

Shader* ResourceManager::LoadShader(ShaderType type, const std::string& vs, const std::string& fs, const const std::string& resource_key, const std::string& gs)
{
    assert(resource_key != "");
    assert(vs != "");
    assert(fs != "");

    std::string vertexCode;
    std::string fragmentCode;

    std::string vs_path = ResourceManager::SHADERS_PATH + vs;
    std::string fs_path = ResourceManager::SHADERS_PATH + fs;

    std::string gs_path = "";

    if (!gs.empty())
        gs_path = ResourceManager::SHADERS_PATH + gs;

    if (loaded_shaders.contains(resource_key)) {
        std::cout << "ERROR::SHADER: Shader with such resource key already exists" << std::endl;
        assert(false);
        return nullptr;
    }

    Shader shader = *new Shader(type, vs_path.c_str(), fs_path.c_str());

    loaded_shaders.insert(std::pair(resource_key, shader));

    return &loaded_shaders.at(resource_key);
}

AtlasTextureInfo GetTextureInfoByIndex(u16 index, v2 icon_size, const std::string& atlas_key)
{
    auto info = AtlasTextureInfo();

    auto atlas = RM.GetTexture(atlas_key);

    u8 row_size = atlas->Width / icon_size.x;
    u8 column_size = atlas->Height / icon_size.y;

    info.position.x = (index % row_size) * icon_size.x;
    info.position.y = (atlas->Height - (index / column_size * icon_size.y)) - icon_size.y;

    info.size = icon_size;
    info.scale = v3(1.0);

    return info;
}

void ResourceManager::HotReloadShaders()
{
    for (auto& pair : loaded_shaders) {
        Shader& shader = pair.second;

        auto vertex_last_written = last_write_time(shader.vertexPath);
        auto fragment_last_written = last_write_time(shader.fragmentPath);

        bool was_vertex_updated = vertex_last_written > shader.vertex_last_written;
        bool was_fragment_updated = fragment_last_written > shader.fragment_last_written;

        if (was_fragment_updated || was_vertex_updated) {
            u32 new_shader_program_id = glCreateProgram();
            u32 v_id, f_id;

            bool v_success = shader.Load(new_shader_program_id, shader.vertexPath, GL_VERTEX_SHADER, "VERTEX", v_id);
            bool f_success = shader.Load(new_shader_program_id, shader.fragmentPath, GL_FRAGMENT_SHADER, "FRAGMENT", f_id);

            shader.vertex_last_written = vertex_last_written;
            shader.fragment_last_written = fragment_last_written;

            if (!v_success || !f_success)
                return;

            glDeleteProgram(shader.ID);
            shader.ID = new_shader_program_id;

            glLinkProgram(shader.ID);

            glDeleteShader(f_id);
            glDeleteShader(v_id);
        }
    }
}
