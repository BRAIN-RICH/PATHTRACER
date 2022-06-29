#ifndef RAYTRACER_INTERSECTION_H
#define RAYTRACER_INTERSECTION_H

#include "enums.h"
#include "vector_type.h"

namespace scg
{

struct Intersection
{
    // Intersection information
    int objectID;//����id�������㣿
    Vec3f position;
    float distance;

    // Geometry normal at intersection point
    Vec3f normal;//�ཻ�㼸������

    // Material information
    SurfaceType surfaceType;//��������
    size_t materialID;//����
    Vec2f uv;
};

}
#endif //RAYTRACER_INTERSECTION_H
