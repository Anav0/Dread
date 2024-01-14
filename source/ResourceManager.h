#ifndef RM_H
#define RM_H

#pragma once

#include <map>
#include <string>

#include "Mesh.h"
#include "Model.h"

class Shader;
class Texture;

using std::string;

class ResourceManager {
private:
    std::string ASSETS_PATH;
    std::string SHADERS_PATH;

    std::map<std::string, Shader>        loaded_shaders;
    std::map<std::string, Texture>       loaded_textures;
    std::map<std::string, Model>         loaded_models;

    unsigned int TextureFromFile(const char *path, const string &directory, bool gamma);

public:

    ResourceManager()
    {
        // TODO: Replace with relative path
        ASSETS_PATH = "D://Projects//Dread//assets//";
        SHADERS_PATH = "D://Projects//Dread//shaders//";
    }

    void LoadAllResources();

    Texture*            GetTexture(std::string resource_key);
    Shader*             GetShader(std::string shader_name);
    Mesh*               LoadMesh(std::string file_path, std::string resource_key, bool gamma = false);
    void                LoadModel(std::string file_path, std::string resource_key, bool gamma = false);
    Model*              GetModel(std::string resource_key);
    vector<std::string> LoadMaterialTextures(aiMaterial* mat, aiTextureType type, const string typeName, const string& directory);
    Texture*            LoadTexture(std::string file_path, std::string resource_key, bool alpha = false);
    Shader*             LoadShader(std::string vs, std::string fs, const std::string resource_key, std::string gs = "");
    Texture*            GetTextureByKey(std::string);
                    
};

extern ResourceManager RM;

#endif
