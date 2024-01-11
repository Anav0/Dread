#include "Buffers.h"
#include "Mesh.h"
#include "Model.h"
#include "ResourceManager.h"

ModelInBuffer MeshBuffer::RenderModel(std::string model_index, v3 position, v3 size, f32 rotation)
{
    ModelInBuffer model_in_buffer;
    this->matrices[rolling_index] = GetTransformMatrix(position, size, rotation);
    Model* model = RM.GetModel(model_index);

    for (auto mesh : model->meshes) {
        auto mesh_in_buffer = MeshInBuffer();
        mesh_in_buffer.mesh_data_index = this->mesh_id_by_vertices_index[mesh->id];
        this->colors[rolling_index] = v4(1.0, 1.0, 1.0, 1.0);
        this->rolling_index++;
        mesh_in_buffer.color_data_index = this->rolling_index - 1;
        model_in_buffer.meshes.push_back(mesh_in_buffer);
    }

    return model_in_buffer;
}