#ifndef MODEL_H
#define MODEL_H

#pragma once

#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>

#include "Constants.h"
#include "Mesh.h"
#include "Texture.h"

#include <fstream>
#include <iostream>
#include <map>
#include <sstream>
#include <string>
#include <vector>

#include "stb_image.h"

using std::string;
using std::vector;

class Shader;

class Model {
public:
    // TODO: use resource manager
    vector<Texture> textures_loaded;
    vector<Mesh> meshes;
    string directory;
    bool gammaCorrection;

    Model() { }

    Model(string const& path, bool gamma = false)
        : gammaCorrection(gamma)
    {
        LoadModel(path);
    }

    void Draw(Shader* shader, m4* projection);

private:
    unsigned int TextureFromFile(const char* path, const string& directory, bool gamma = false);
    void LoadModel(string const& path);
    Mesh ProcessMesh(aiMesh* mesh, const aiScene* scene);
    void ProcessNode(aiNode* node, const aiScene* scene);
    vector<Texture> LoadMaterialTextures(aiMaterial* mat, aiTextureType type, string typeName);
};

#endif
