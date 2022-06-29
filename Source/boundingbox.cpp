﻿#include "boundingbox.h"

#include "ray.h"

#include <cmath>

namespace scg
{
//传入包围盒两个坐标
BoundingBox::BoundingBox(Vec3f const& min, Vec3f const& max)
{
    this->min = min;
    this->max = max;
    this->mid = (min + max) / 2.0f;
}

//传入光线，判断相交性
void BoundingBox::getIntersection(Ray const& ray, BBIntersection &intersection) const
{
    intersection.valid = false;

	//NaN：未定义或不可表示的值
	//Inf:一个集合最大的下界
    // Assume NaN can only occur if INF(invR) * 0(dist) => intersect because dist = 0
    Vec3f invR		= Vec3f(1.0f) / ray.direction;//光线方向的逆
    Vec3f dMin = this->min - ray.origin;
    Vec3f dMax = this->max - ray.origin;
    
    Vec3f bottomT = invR * dMin;
    bottomT.x = dMin.x == 0 ? 0 : bottomT.x;
    bottomT.y = dMin.y == 0 ? 0 : bottomT.y;
    bottomT.z = dMin.z == 0 ? 0 : bottomT.z;
    
    Vec3f topT = invR * dMax;
    topT.x = dMax.x == 0 ? 0 : topT.x;
    topT.y = dMax.y == 0 ? 0 : topT.y;
    topT.z = dMax.z == 0 ? 0 : topT.z;
    
    Vec3f minT		= minV(topT, bottomT);//TopT.Min(BottomT);
    Vec3f maxT		= maxV(topT, bottomT);//TopT.Max(BottomT);
    float largestMinT = fmaxf(fmaxf(minT.x, minT.y), minT.z);
    float smallestMaxT = fminf(fminf(maxT.x, maxT.y), maxT.z);

    if (smallestMaxT < largestMinT)
        return;

    intersection.nearT	= largestMinT;
    intersection.farT	= smallestMaxT;

    if (intersection.farT < ray.minT || intersection.nearT > ray.maxT)
        return;

    intersection.valid	= true;
}

}