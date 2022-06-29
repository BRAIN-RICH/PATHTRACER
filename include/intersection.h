#ifndef RAYTRACER_INTERSECTION_H
#define RAYTRACER_INTERSECTION_H

#include "enums.h"
#include "vector_type.h"

namespace scg
{

struct Intersection
{
    // Intersection information
    int objectID;//物体id，采样点？
    Vec3f position;
    float distance;

    // Geometry normal at intersection point
    Vec3f normal;//相交点几何正交

    // Material information
    SurfaceType surfaceType;//表面类型
    size_t materialID;//材质
    Vec2f uv;
};

}
#endif //RAYTRACER_INTERSECTION_H
