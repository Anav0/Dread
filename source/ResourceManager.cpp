#include <filesystem>
#include <map>
#include <string>

#include "ResourceManager.h"

void ResourceManager::LoadAllResources() {
    for (const auto&entry: std::filesystem::recursive_directory_iterator(ASSETS_PATH)) {
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

Texture* ResourceManager::GetTexture(std::string resource_key) {
    if (!loaded_textures.contains(resource_key))
        return nullptr;

    return &loaded_textures[resource_key];
}

Shader* ResourceManager::GetShader(std::string shader_name) {
    return &loaded_shaders[shader_name];
}

Texture* ResourceManager::LoadTexture(std::string file_path, std::string resource_key, bool alpha) {
    if (file_path.find(ASSETS_PATH) == std::string::npos) {
        file_path = ASSETS_PATH + file_path;
    }

    Texture texture;
    if (alpha) {
        texture.Internal_Format = GL_RGBA;
        texture.Image_Format    = GL_RGBA;
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
        std::cout << "ERROR::LoadTexture: Failed to save loaded texture, resource with such name already exists" <<
                std::endl;
        abort();
    }

    std::cout << "ResourceManager::LoadTexture -> Loaded texture: " + file_path << std::endl;

    loaded_textures.insert(std::pair(resource_key, texture));

    return &loaded_textures[resource_key];
}

Shader* ResourceManager::LoadShader(std::string vs, std::string fs, const std::string resource_key) {
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
