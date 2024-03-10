#ifndef RM_H
#define RM_H

#pragma once

#include <map>
#include <string>
#include <algorithm>

#include "Mesh.h"
#include "Model.h"

class Shader;
class Texture;

using std::string;

static std::string ASSETS_PATH = "D:/Projects/Dread/Game/Assets/";
static std::string FONTS_PATH  = "D:/Projects/Dread/Game/Fonts";

class ResourceManager {
private:
    std::string SHADERS_PATH;

    std::map<std::string, Shader>        loaded_shaders;
    std::map<std::string, Texture>       loaded_textures;
    std::map<std::string, Model>         loaded_models;

    u32 TextureFromFile(const char *path, const string& directory, bool gamma);

public:

    ResourceManager()
    {
        SHADERS_PATH = "D://Projects//Dread//Game//Shaders//";
    }

    void LoadAllResources();
  	void LoadRequiredResources();

    Texture*            GetTexture(const std::string& resource_key);
    Shader*             GetShader(const std::string& shader_name);
    Mesh*               LoadMesh(const std::string& file_path, const std::string& resource_key, bool gamma = false);
    void                LoadModel(const std::string& file_path, const std::string& resource_key, bool gamma = false);
    Model*              GetModel(const std::string& resource_key);
    vector<std::string> LoadMaterialTextures(aiMaterial* mat, aiTextureType type, const string& typeName, const string& directory);
    Texture*            LoadTexture(std::string file_path, const std::string& resource_key, bool absolute_path = false, bool alpha = false);
    Shader*             LoadShader(const std::string& vs, const std::string& fs, const std::string& resource_key, const std::string& gs = "");
	void                HotReloadShaders();
    Texture*            GetTextureByKey(const std::string&);
                    
};

// Returns texture info eg. pos, width and height based on it's index in atlas
AtlasTextureInfo GetTextureInfoByIndex(u16 index, v2 icon_size, const std::string& atlas_key);

extern ResourceManager RM;


#endif
