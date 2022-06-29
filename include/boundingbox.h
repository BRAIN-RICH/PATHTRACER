#ifndef RAYTRACER_BOUNDINGBOX_H
#define RAYTRACER_BOUNDINGBOX_H

#include "ray.h"
#include "vector_type.h"

#include <limits>

namespace scg
{
//包围盒交叉
class BBIntersection
{
public:
    BBIntersection()
    {
        this->SetInvalid();
    }

    void SetValid(float NearT)
    {
        this->valid		= true;
        this->nearT		= NearT;
    }

    void SetInvalid()
    {
        this->valid		= false;
        this->front		= true;
        this->nearT		= 0.0f;
        this->farT		= std::numeric_limits<float>::max();
    }

	//重载赋值符号，使包围盒间能互相赋值
    BBIntersection& operator = (BBIntersection const& Other)
    {
        this->valid			= Other.valid;
        this->front			= Other.front;
        this->nearT			= Other.nearT;
        this->farT			= Other.farT;

        return *this;
    }

    bool	valid;
    bool	front;
    float	nearT;
    float	farT;
};

class BoundingBox
{
public:
    Vec3f min;//盒子最小处
    Vec3f max;
    Vec3f mid;

    BoundingBox() {};

    BoundingBox(Vec3f const&, Vec3f const&);//传入两个关键位置坐标

    void getIntersection(Ray const& ray, BBIntersection &) const;//查看光线是否与包围盒交叉
};

}

#endif //RAYTRACER_BOUNDINGBOX_H
