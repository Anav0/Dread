#include "RenderHelpers.h"
#include "Base.h"
#include "BoundingBox.h"
#include "Collision.h"
#include "EntityManager.h"
#include "Model.h"
#include "Renderer.h"

ModelInBuffer AddModel(Model* model, v3 position, v3 size, u32 buffer_index)
{
    m4 matrix = glm::translate(m4(1.0), position);
    auto buffer = &R.buffers[buffer_index];

    //return buffer->RenderModel(model->name, position, size);
    return ModelInBuffer();
}

ModelInBuffer RenderSphere(v3 position, v3 size, u32 buffer_index)
{
    Model* sphere = RM.LoadModel("sphere/sphere.obj", "sphere");
    return AddModel(sphere, position, size, buffer_index);
}
