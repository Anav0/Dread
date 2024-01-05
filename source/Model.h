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
class Model;

namespace Models {
Mesh TransformMesh(aiMesh* mesh, const aiScene* scene, const string& directory);
void LoadMeshesFromScene(vector<Mesh>* meshes, aiNode* node, const aiScene* scene, const string& directory);
}

class Model {
public:
    vector<Mesh*> meshes;
    bool gamma_correction;

    Model() { }

    Model(bool gamma)
        : gamma_correction(gamma)
    {
    }

    void Draw(Shader* shader, m4* projection);
    void SetMatrixTransform(m4 model);
};

Model* GetModelFromBoundingBoxIndex(int);

#endif
