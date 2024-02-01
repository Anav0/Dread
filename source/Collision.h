#ifndef COLLISION_H
#define COLLISION_H

#pragma once

#include "Base.h"
#include "Constants.h"
#include "BoundingBox.h"

class Mesh;
class MeshInBuffer;
class Model;

struct Collision {
    bool hit_something;
    f32 distance;
    v3 point;
    v3 normal;
    EntityType what_was_hit;
    BoundingBox box;
};

struct Ray {
    v3 position, direction;
};

Collision CheckRayCollision(Ray ray, m4 projection);
Collision CheckRayCollisionWithModels(Ray ray, m4 projection);
Collision CheckRayCollisionWithBoundingBoxes(Ray ray, m4 projection);
Collision CheckBoundingBoxCollision(Ray ray, BoundingBox box);
Collision CheckMeshCollision(Ray* ray, Mesh* mesh, m4 transform);
Collision CheckTriangleCollision(Ray* ray, v3 p1, v3 p2, v3 p3);

void AddBoundingBox(MeshInBuffer mesh, ID child_id);

#endif
