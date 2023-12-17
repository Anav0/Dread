#pragma once

#include <map>
#include <string>
#include "Shader.h"
#include "Texture.h"

class ResourceManager {
private:
    std::string ASSETS_PATH;
    std::string SHADERS_PATH;

    std::map<std::string, Shader> loaded_shaders;
    std::map<std::string, Texture> loaded_textures;

public:
    ResourceManager()
    {
        // TODO: Replace with relative path
        ASSETS_PATH = "D://Projects//Dread//assets//";
        SHADERS_PATH = "D://Projects//Dread//shaders//";
    }

    void LoadAllResources();

    Texture* GetTexture(std::string resource_key);
    Shader* GetShader(std::string shader_name);
    Texture* LoadTexture(std::string file_path, std::string resource_key, bool alpha = false);

    Shader* LoadShader(std::string vs, std::string fs, const std::string resource_key);
};

static ResourceManager RM;
