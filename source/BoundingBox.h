#ifndef BB_H
#define BB_H

#pragma once

#include "Base.h"
#include "Constants.h"
#include "Shader.h"

class BoundingBox {
public:
    v3 size;
    v3 center;
    unsigned int VAO;
    Shader* shader;
    m4 model;
    bool is_dirty;

    BoundingBox()
    {
        this->shader = RM.GetShader("debug");
    }

    BoundingBox(v3 size, v3 center)
    {
        this->center = center;
        this->size = size;

        this->model = glm::translate(m4(1.0), center) * glm::scale(m4(1.0), size);

        this->shader = RM.GetShader("debug");

        setup();
    }

    void Update()
    {
        if (is_dirty) {
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
    unsigned int VBO;

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
