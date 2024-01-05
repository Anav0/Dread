#ifndef COLLISION_H
#define COLLISION_H

#pragma once

#include "Base.h"
#include "Constants.h"

class Mesh;
class BoundingBox;
class Model;

struct Collision {
    bool hit_something;
    float distance;
    v3 point;
    v3 normal;
    EntityType what_was_hit;
    u32 mesh_index;
};

struct Ray {
    v3 position, direction;
};

Collision CheckRayCollision(Ray ray, m4 projection);
Collision CheckRayCollisionWithModels(Ray ray, m4 projection);
Collision CheckRayCollisionWithBoundingBoxes(Ray ray, m4 projection, u32* model_index_in_renderer);
Collision CheckBoundingBoxCollision(Ray ray, BoundingBox box);
Collision CheckMeshCollision(Ray* ray, Mesh* mesh, m4 transform);
Collision CheckTriangleCollision(Ray* ray, v3 p1, v3 p2, v3 p3);

void SetupBoundingBox(Mesh* mesh, u32 parent_model_index);
void CalculateBoundingBoxMinAndMaxPoints(Mesh* mesh, v3* size, v3* center);

#endif
