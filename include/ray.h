#ifndef RAYTRACER_RAY_H
#define RAYTRACER_RAY_H

#include "vector_type.h"

namespace scg
{

class Ray
{
public:
    Vec3f origin;//三维原点
    Vec3f direction;//三维方向
    float minT;//起始时间
    float maxT;//结束时间

    Ray() = default;

	//根据原点，方向，起始时间，结束时间创建光线
    Ray(Vec3f const& origin, Vec3f const& direction, float minT = 0, float maxT = INF):
        origin(origin), direction(normalise(direction)), minT(minT), maxT(maxT) {};

	//重载函数调用运算符
    inline Vec3f operator()(float const t) const
    {
        return origin + direction * t;//返回原点+光线*时间，即光线位置
    }

	//判断光线是否还在盒子中
    inline bool isInside(float distance) const
    {
        return minT <= distance && distance <= maxT;
    }
};

}

#endif //RAYTRACER_RAY_H
