#include "Collision.h"
#include "BoundingBox.h"
#include "Renderer.h"
#include "ResourceManager.h"

inline v3 TransformV3(v3 v, m4 mat)
{
    return mat * v4(v, 1.0);
}

inline v3 Vector3Subtract(v3 v1, v3 v2)
{
    return { v1.x - v2.x, v1.y - v2.y, v1.z - v2.z };
}
inline v3 Vector3CrossProduct(v3 v1, v3 v2)
{
    return { v1.y * v2.z - v1.z * v2.y, v1.z * v2.x - v1.x * v2.z, v1.x * v2.y - v1.y * v2.x };
}

inline f32 Vector3DotProduct(v3 v1, v3 v2)
{
    return (v1.x * v2.x + v1.y * v2.y + v1.z * v2.z);
}

inline v3 Vector3Lerp(v3 v1, v3 v2, f32 amount)
{
    v3 result;

    result.x = v1.x + amount * (v2.x - v1.x);
    result.y = v1.y + amount * (v2.y - v1.y);
    result.z = v1.z + amount * (v2.z - v1.z);

    return result;
}
inline v3 Vector3Normalize(v3 v)
{
    v3 result = v;

    f32 length = sqrtf(v.x * v.x + v.y * v.y + v.z * v.z);
    if (length != 0.0f) {
        f32 ilength = 1.0f / length;

        result.x *= ilength;
        result.y *= ilength;
        result.z *= ilength;
    }

    return result;
}

inline v3 Vector3Add(v3 v1, v3 v2)
{
    return { v1.x + v2.x, v1.y + v2.y, v1.z + v2.z };
}

inline v3 Vector3Scale(v3 v, f32 scalar)
{
    return { v.x * scalar, v.y * scalar, v.z * scalar };
}

inline v3 Vector3Divide(v3 v1, v3 v2)
{
    return { v1.x / v2.x, v1.y / v2.y, v1.z / v2.z };
}

inline v3 Vector3Negate(v3 v)
{
    return { -v.x, -v.y, -v.z };
}

Collision CheckRayCollision(Ray ray, m4 projection)
{
    Collision collision = CheckRayCollisionWithBoundingBoxes(ray, projection);

    if (!collision.hit_something)
        return collision;

    auto mesh_in_buffer = collision.box.mesh_info;
    mesh_in_buffer.ChangeColor(RED);
    return collision;
}

Collision CheckRayCollisionWithModels(Ray ray, m4 projection)
{
    Collision collision;
    collision.hit_something = false;

    /* for (auto model : R.models) {
         for (auto mesh : model->meshes) {
             Collision c = CheckMeshCollision(&ray, mesh, projection);
             if (c.hit_something) {
                 return c;
             }
         }
     }*/
    return collision;
}

Collision CheckRayCollisionWithBoundingBoxes(Ray ray, m4 projection)
{
    Collision collision;
    collision.hit_something = false;

    u32 i = 0;
    for (auto const& box : R.boxes) {
        Collision c = CheckBoundingBoxCollision(ray, box);
        if (c.hit_something) {
            printf("Bounding box hit! ID: %i\n", i);
            c.box = box;
            return c;
        }
        i++;
    }
    return collision;
}

Collision CheckBoundingBoxCollision(Ray ray, BoundingBox box)
{
    Collision collision;

    // Note: If ray.position is inside the box, the distance is negative (as if the ray was reversed)
    // Reversing ray.direction will give use the correct result.
    bool insideBox = (ray.position.x > box.min.x) && (ray.position.x < box.max.x) && (ray.position.y > box.min.y) && (ray.position.y < box.max.y) && (ray.position.z > box.min.z) && (ray.position.z < box.max.z);

    if (insideBox)
        ray.direction = Vector3Negate(ray.direction);

    f32 t[11] = { 0 };

    t[8] = 1.0f / ray.direction.x;
    t[9] = 1.0f / ray.direction.y;
    t[10] = 1.0f / ray.direction.z;

    t[0] = (box.min.x - ray.position.x) * t[8];
    t[1] = (box.max.x - ray.position.x) * t[8];
    t[2] = (box.min.y - ray.position.y) * t[9];
    t[3] = (box.max.y - ray.position.y) * t[9];
    t[4] = (box.min.z - ray.position.z) * t[10];
    t[5] = (box.max.z - ray.position.z) * t[10];
    t[6] = (f32)fmax(fmax(fmin(t[0], t[1]), fmin(t[2], t[3])), fmin(t[4], t[5]));
    t[7] = (f32)fmin(fmin(fmax(t[0], t[1]), fmax(t[2], t[3])), fmax(t[4], t[5]));

    collision.hit_something = !((t[7] < 0) || (t[6] > t[7]));
    collision.distance = t[6];
    collision.point = Vector3Add(ray.position, Vector3Scale(ray.direction, collision.distance));

    // Get box center point
    collision.normal = Vector3Lerp(box.min, box.max, 0.5f);

    // Get vector center point->hit point
    collision.normal = Vector3Subtract(collision.point, collision.normal);
    // Scale vector to unit cube
    // NOTE: We use an additional .01 to fix numerical errors
    collision.normal = Vector3Scale(collision.normal, 2.01f);
    collision.normal = Vector3Divide(collision.normal, Vector3Subtract(box.max, box.min));
    // The relevant elements of the vector are now slightly larger than 1.0f (or smaller than -1.0f)
    // and the others are somewhere between -1.0 and 1.0 casting to int is exactly our wanted normal!
    collision.normal.x = (f32)((int)collision.normal.x);
    collision.normal.y = (f32)((int)collision.normal.y);
    collision.normal.z = (f32)((int)collision.normal.z);

    collision.normal = Vector3Normalize(collision.normal);

    if (insideBox) {
        // Reset ray.direction
        ray.direction = Vector3Negate(ray.direction);
        // Fix result
        collision.distance *= -1.0f;
        collision.normal = Vector3Negate(collision.normal);
    }

    return collision;
}

Collision CheckMeshCollision(Ray* ray, Mesh* mesh, m4 transform)
{
    Collision closest_colision {};

    for (int i = 0; i < mesh->triangle_count; i++) {
        v3 a = mesh->vertices[i].Position;
        v3 b = mesh->vertices[i + 1].Position;
        v3 c = mesh->vertices[i + 2].Position;

        a = TransformV3(a, transform);
        b = TransformV3(b, transform);
        c = TransformV3(c, transform);

        Collision triHitInfo = CheckTriangleCollision(ray, a, b, c);

        if (triHitInfo.hit_something) {
            if ((!closest_colision.hit_something) || (closest_colision.distance > triHitInfo.distance))
                closest_colision = triHitInfo;
        }
    }
    return closest_colision;
}

Collision CheckTriangleCollision(Ray* ray, v3 p1, v3 p2, v3 p3)
{
#define EPSILON 0.000001f // A small number

    Collision collision = { 0 };
    v3 edge1;
    v3 edge2;
    v3 p, q, tv;
    f32 det, invDet, u, v, t;

    // Find vectors for two edges sharing V1
    edge1 = Vector3Subtract(p2, p1);
    edge2 = Vector3Subtract(p3, p1);

    // Begin calculating determinant - also used to calculate u parameter
    p = Vector3CrossProduct(ray->direction, edge2);

    // If determinant is near zero, ray lies in plane of triangle or ray is parallel to plane of triangle
    det = Vector3DotProduct(edge1, p);

    // Avoid culling!
    if ((det > -EPSILON) && (det < EPSILON))
        return collision;

    invDet = 1.0f / det;

    // Calculate distance from V1 to ray origin
    tv = Vector3Subtract(ray->position, p1);

    // Calculate u parameter and test bound
    u = Vector3DotProduct(tv, p) * invDet;

    // The intersection lies outside the triangle
    if ((u < 0.0f) || (u > 1.0f))
        return collision;

    // Prepare to test v parameter
    q = Vector3CrossProduct(tv, edge1);

    // Calculate V parameter and test bound
    v = Vector3DotProduct(ray->direction, q) * invDet;

    // The intersection lies outside the triangle
    if ((v < 0.0f) || ((u + v) > 1.0f))
        return collision;

    t = Vector3DotProduct(edge2, q) * invDet;

    if (t > EPSILON) {
        // Ray hit, get hit point and normal
        collision.hit_something = true;
        collision.distance = t;
        collision.normal = Vector3Normalize(Vector3CrossProduct(edge1, edge2));
        collision.point = Vector3Add(ray->position, Vector3Scale(ray->direction, t));
    }

    return collision;
}

void SetupBoundingBox(MeshInBuffer mesh)
{
    auto buffer = R.GetBufferByIndex(mesh.buffer_index);

    auto& vertices = buffer->mesh.vertices;
    f32 max_x = vertices[0].Position.x;
    f32 max_y = vertices[0].Position.y;
    f32 max_z = vertices[0].Position.z;

    f32 min_x = vertices[0].Position.x;
    f32 min_y = vertices[0].Position.y;
    f32 min_z = vertices[0].Position.z;

    for (size_t i = 0; i < vertices.size(); i++) {
        Vertex* v = &vertices[i];

        if (v->Position.x < min_x)
            min_x = v->Position.x;
        if (v->Position.y < min_y)
            min_y = v->Position.y;
        if (v->Position.z < min_z)
            min_z = v->Position.z;

        if (v->Position.x > max_x)
            max_x = v->Position.x;
        if (v->Position.y > max_y)
            max_y = v->Position.y;
        if (v->Position.z > max_z)
            max_z = v->Position.z;
    }

    v3 min = TransformV3({ min_x, min_y, min_z }, mesh.GetMatrix());
    v3 max = TransformV3({ max_x, max_y, max_z }, mesh.GetMatrix());

    auto box = BoundingBox(min, max, mesh);
    auto key = mesh.DeriveKeyFromIndexes();
    R.boxes.push_back(box);
}
