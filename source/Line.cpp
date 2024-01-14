#include "Line.h"
#include "GameState.h"
#include "Shader.h"

Line::Line(v3 from, v3 to)
{
    this->points.push_back(from);
    this->points.push_back(to);
    this->model = m4(1.0);

    Setup();
}

void Line::Setup()
{
    auto size = this->points.size() * sizeof(v3);

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, size, &points[0], GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(v3), (void*)0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void Line::Draw(Shader* shader, m4* projection)
{
    glBindVertexArray(VAO);
    shader->Use();
    shader->setMat4("view", STATE.window.camera.GetViewMatrix());
    shader->setMat4("projection", *projection);
    shader->setMat4("model", this->model);
    glDrawArrays(GL_LINES, 0, points.size());
    glBindVertexArray(0);
}

void Line::Redo(v3 from, v3 to)
{
    this->points.clear();
    this->points.push_back(from);
    this->points.push_back(to);
    this->model = m4(1.0);

    Setup();
}
