#include "Buffers.h"
#include "Mesh.h"
#include "Model.h"
#include "ResourceManager.h"

MeshBuffer::MeshBuffer(std::vector<std::string>& models, ID id)
{
    rolling_index = 0;

    for (auto& name : models) {
        auto model = RM.GetModel(name);
        for (auto& mesh : model->meshes) {
            assert(mesh.vertices.size() > 0);

            u32 index = vertices.size();
            vertices.insert(vertices.end(), mesh.vertices.begin(), mesh.vertices.end());
            assert(!mesh_id_by_vertices_index.contains(mesh.id));
            mesh_id_by_vertices_index.insert(std::pair(mesh.id, index));

            index = indices.size();
            indices.insert(indices.end(), mesh.indices.begin(), mesh.indices.end());
            assert(!mesh_id_by_indices_index.contains(mesh.id));
            mesh_id_by_indices_index.insert(std::pair(mesh.id, index));
        }
    }

    AllocateBufferData();
    Initialize();
}

ModelInBuffer MeshBuffer::RenderModel(std::string model_index, v3 position, v3 size, f32 rotation)
{
    ModelInBuffer model_in_buffer;
    this->matrices[rolling_index] = GetTransformMatrix(position, size, rotation);
    Model* model = RM.GetModel(model_index);

    for (auto mesh : model->meshes) {
        auto mesh_in_buffer = MeshInBuffer();
        mesh_in_buffer.mesh_data_index = this->mesh_id_by_vertices_index[mesh.id];
        this->colors[rolling_index] = v4(1.0, 1.0, 1.0, 1.0);
        this->rolling_index++;
        mesh_in_buffer.color_data_index = this->rolling_index - 1;
        model_in_buffer.meshes.push_back(mesh_in_buffer);
    }

    return model_in_buffer;
}