#ifndef RAYTRACER_INTERACTION_H
#define RAYTRACER_INTERACTION_H

#include "enums.h"
#include "vector_type.h"

namespace scg
{

// Information regarding an interaction with the scene
// May not initialise all fields and update them during the process
//散射事件
class ScatterEvent
{
public:
    Vec3f position;//位置
    Vec3f normal;//正交化
    Vec2f uv;

    Vec3f inputDir;//输入方向
    Vec3f outputDir;//输出方向

    float iorI;//各向同性isotropy
    float iorO;//各向异性anisotropy

    SurfaceType surfaceType;//表面类型
    BSDFLobe sampledLobe;//采样叶

    ScatterEvent() = default;

    ScatterEvent(Vec3f const& position, Vec3f const& normal, SurfaceType surfaceType):
        position(position), normal(normal), surfaceType(surfaceType) {};

	
    Vec3f getSafePosition() const
    {
        return position + normal * EPS;
    }
};

}

#endif //RAYTRACER_INTERACTION_H
