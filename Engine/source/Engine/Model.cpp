#include "Model.h"
#include "Mesh.h"
#include "Renderer.h"
#include "ResourceManager.h"
#include "Shader.h"

void Model::Draw(Shader* shader, m4 projection, m4 view)
{
    shader->Use();
    for (u32 i = 0; i < meshes.size(); i++)
        meshes[i].Draw(shader, projection, GetTransformMatrix({ 0, 0, 0 }, { 10, 10, 10 }), view);
}

Mesh Models::TransformMesh(aiMesh* mesh, const aiScene* scene, const string& directory)
{
    vector<Vertex> vertices;
    vector<u32> indices;
    vector<std::string> texture_keys;

    for (u32 i = 0; i < mesh->mNumVertices; i++) {
        Vertex vertex;
        v3 vector;
        vector.x = mesh->mVertices[i].x;
        vector.y = mesh->mVertices[i].y;
        vector.z = mesh->mVertices[i].z;
        vertex.Position = vector;

        if (mesh->HasNormals()) {
            vector.x = mesh->mNormals[i].x;
            vector.y = mesh->mNormals[i].y;
            vector.z = mesh->mNormals[i].z;
            vertex.Normal = vector;
        }

        if (mesh->mTextureCoords[0]) {
            v2 vec;

            vec.x = mesh->mTextureCoords[0][i].x;
            vec.y = mesh->mTextureCoords[0][i].y;
            vertex.TexCoords = vec;

            //vector.x = mesh->mTangents[i].x;
            //vector.y = mesh->mTangents[i].y;
            //vector.z = mesh->mTangents[i].z;
            //vertex.Tangent = vector;

            //vector.x = mesh->mBitangents[i].x;
            //vector.y = mesh->mBitangents[i].y;
            //vector.z = mesh->mBitangents[i].z;
            //vertex.Bitangent = vector;
        } else
            vertex.TexCoords = v2(-1.0f, -1.0f);

        vertices.push_back(vertex);
    }
    for (u32 i = 0; i < mesh->mNumFaces; i++) {
        aiFace face = mesh->mFaces[i];
        for (u32 j = 0; j < face.mNumIndices; j++)
            indices.push_back(face.mIndices[j]);
    }

    aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];

    auto diffuseMaps = RM.LoadMaterialTextures(material, aiTextureType_DIFFUSE, "texture_diffuse", directory);
    texture_keys.insert(texture_keys.end(), diffuseMaps.begin(), diffuseMaps.end());

    auto specularMaps = RM.LoadMaterialTextures(material, aiTextureType_SPECULAR, "texture_specular", directory);
    texture_keys.insert(texture_keys.end(), specularMaps.begin(), specularMaps.end());

    auto normalMaps = RM.LoadMaterialTextures(material, aiTextureType_HEIGHT, "texture_normal", directory);
    texture_keys.insert(texture_keys.end(), normalMaps.begin(), normalMaps.end());

    auto heightMaps = RM.LoadMaterialTextures(material, aiTextureType_AMBIENT, "texture_height", directory);
    texture_keys.insert(texture_keys.end(), heightMaps.begin(), heightMaps.end());

    return Mesh(mesh->mNumVertices / 3, vertices, indices, texture_keys);
}

void Models::LoadMeshesFromScene(vector<Mesh>& meshes, aiNode* node, const aiScene* scene, const string& directory)
{
    for (u32 i = 0; i < node->mNumMeshes; i++) {
        aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
        Mesh m = TransformMesh(mesh, scene, directory);
        meshes.push_back(m);
    }

    for (u32 i = 0; i < node->mNumChildren; i++) {
        LoadMeshesFromScene(meshes, node->mChildren[i], scene, directory);
    }
}

Model* GetModelFromBoundingBoxIndex(int box_index)
{
    // return R.models[box_index];
    return nullptr;
}
