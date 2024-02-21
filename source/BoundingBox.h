#ifndef BB_H
#define BB_H

#pragma once

#include <algorithm>

#include "Base.h"
#include "Constants.h"
#include "GameState.h"
#include "ResourceManager.h"
#include "Shader.h"

inline v3 Vector3Lerp(v3 v1, v3 v2, f32 amount);

class BoundingBox {
public:
    v3 min, max;
    u32 VAO;
    Shader* shader;
    m4 model;
    bool is_dirty;

    ID child_id;

    void Init() 
    {
        this->shader = RM.GetShader("debug");
    }

    BoundingBox() { }

    BoundingBox(v3 min, v3 max, ID child_id)
    {
        this->max = max;
        this->min = min;
        this->child_id = child_id;

        v3 size;
        size.x = fabsf(max.x - min.x);
        size.y = fabsf(max.y - min.y);
        size.z = fabsf(max.z - min.z);

        v3 center = { min.x + size.x / 2.0f, min.y + size.y / 2.0f, min.z + size.z / 2.0f };

        this->model = glm::translate(m4(1.0), center) * glm::scale(m4(1.0), size);
        this->shader = RM.GetShader("debug");

        setup();
    }

    void Update()
    {
        if (is_dirty) {
            v3 size;
            size.x = fabsf(max.x - min.x);
            size.y = fabsf(max.y - min.y);
            size.z = fabsf(max.z - min.z);

            v3 center = { min.x + size.x / 2.0f, min.y + size.y / 2.0f, min.z + size.z / 2.0f };

            this->model = glm::translate(m4(1.0), center) * glm::scale(m4(1.0), size);
        }

        is_dirty = false;
    }

    void Draw(m4* projection)
    {
        this->shader->Use();
        this->shader->setMat4("projection", *projection);
        this->shader->setMat4("view", STATE.window.camera.GetViewMatrix());
        this->shader->setMat4("model", model);

        DrawInner();
    }

    void Draw(Shader* shader, m4* projection)
    {
        shader->Use();
        shader->setMat4("projection", *projection);
        shader->setMat4("view", STATE.window.camera.GetViewMatrix());
        shader->setMat4("model", model);

        DrawInner();
    }

private:
    u32 VBO;

    void setup()
    {
        f32 vertices[] = {
            -0.5f,
            -0.5f,
            -0.5f,
            0.5f,
            -0.5f,
            -0.5f,
            0.5f,
            0.5f,
            -0.5f,
            0.5f,
            0.5f,
            -0.5f,
            -0.5f,
            0.5f,
            -0.5f,
            -0.5f,
            -0.5f,
            -0.5f,

            -0.5f,
            -0.5f,
            0.5f,
            0.5f,
            -0.5f,
            0.5f,
            0.5f,
            0.5f,
            0.5f,
            0.5f,
            0.5f,
            0.5f,
            -0.5f,
            0.5f,
            0.5f,
            -0.5f,
            -0.5f,
            0.5f,

            -0.5f,
            0.5f,
            0.5f,
            -0.5f,
            0.5f,
            -0.5f,
            -0.5f,
            -0.5f,
            -0.5f,
            -0.5f,
            -0.5f,
            -0.5f,
            -0.5f,
            -0.5f,
            0.5f,
            -0.5f,
            0.5f,
            0.5f,

            0.5f,
            0.5f,
            0.5f,
            0.5f,
            0.5f,
            -0.5f,
            0.5f,
            -0.5f,
            -0.5f,
            0.5f,
            -0.5f,
            -0.5f,
            0.5f,
            -0.5f,
            0.5f,
            0.5f,
            0.5f,
            0.5f,

            -0.5f,
            -0.5f,
            -0.5f,
            0.5f,
            -0.5f,
            -0.5f,
            0.5f,
            -0.5f,
            0.5f,
            0.5f,
            -0.5f,
            0.5f,
            -0.5f,
            -0.5f,
            0.5f,
            -0.5f,
            -0.5f,
            -0.5f,

            -0.5f,
            0.5f,
            -0.5f,
            0.5f,
            0.5f,
            -0.5f,
            0.5f,
            0.5f,
            0.5f,
            0.5f,
            0.5f,
            0.5f,
            -0.5f,
            0.5f,
            0.5f,
            -0.5f,
            0.5f,
            -0.5f,
        };

        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);

        glBindVertexArray(VAO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), &vertices[0], GL_STATIC_DRAW);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(v3), (void*)0);
        glEnableVertexAttribArray(0);

        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }

    void DrawInner()
    {
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        glBindVertexArray(VAO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);

        glDrawArrays(GL_TRIANGLES, 0, 36);

        glBindVertexArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }
};

#endif
