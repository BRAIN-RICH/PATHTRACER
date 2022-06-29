﻿#ifndef RAYTRACER_GEOMETRY_H
#define RAYTRACER_GEOMETRY_H

#include "boundingbox.h"
#include "intersection.h"
#include "math_vector_utils.h"
#include "ray.h"
#include "sampler.h"
#include "scatterevent.h"
#include "triangle.h"

#include <cassert>
#include <vector>

namespace scg
{
//几何虚基类
class Geometry
{
public:
    virtual bool getIntersection(Ray const&, Intersection&) const = 0;
    virtual ScatterEvent sampleSurface(Sampler&) const = 0;
    virtual BoundingBox getBoundingBox() const = 0;
};
//球类
class Sphere : public Geometry
{
public:
    float radius;

    size_t materialID;//size_t表示C中对象能达到的最大长度,

    Sphere(float radius, size_t materialID):
        radius(radius), materialID(materialID) {};

	//判断是否相交
    bool getIntersection(Ray const& ray, Intersection& intersection) const override
    {
        //std::cout << "Circle" << std::endl;
        float b = dot(ray.origin * 2.0f, ray.direction);
        float c = dot(ray.origin, ray.origin) - radius * radius;
        float disc = b * b - 4 * c;

        if (disc < 0)
            return false;

        disc = std::sqrt(disc);
        float sol1 = (-b + disc) / 2.0f;
        float sol2 = (-b - disc) / 2.0f;

        float t;
        //if (sol2 > EPS)
        if (ray.isInside(sol2))
            t = sol2;
        //else if (sol1 > EPS)
        else if (ray.isInside(sol1))
            t = sol1;
        else
            return false;

        intersection.position   = ray(t);
        intersection.distance   = t;
        intersection.normal     = normalise(intersection.position);
        intersection.surfaceType = SurfaceType::Surface;
        intersection.materialID = materialID;
        intersection.uv         = Vec2f(0, 0);

        return true;
    }

    ScatterEvent sampleSurface(Sampler &sampler) const override
    {
        Vec3f point = sampleSphere(sampler);
        return ScatterEvent{point * radius, point, SurfaceType::Surface};
    }

    BoundingBox getBoundingBox() const
    {
        Vec3f min = Vec3f(-1) * radius;
        Vec3f max = Vec3f(1) * radius;

        return BoundingBox(min, max);
    }
};

//网状物
class Mesh : public Geometry
{
public:
    std::vector<Triangle> triangles;

    Mesh() = default;

    Mesh(std::vector<Triangle>& triangles):
        triangles(std::move(triangles)) {};

	//判断射线是否与三角面片相交，利用三角形重心坐标计算出光线是否与三角形相交，M-T算法。
    bool getIntersection(Ray const& ray, Intersection& intersection) const override
    {
        //std::cout << "Mesh" << std::endl;
        //Möller–Trumbore intersection algorithm
        float minDistance = std::numeric_limits<float>::max();
        int index = -1;

        for (int i = 0; i < (int)triangles.size(); ++i)
        {
            Vec3f vertex0 = triangles[i].v0;
            Vec3f vertex1 = triangles[i].v1;
            Vec3f vertex2 = triangles[i].v2;
            Vec3f edge1, edge2, h, s, q;
            float a, f, u, v;

            edge1 = vertex1 - vertex0;
            edge2 = vertex2 - vertex0;

            h = cross(ray.direction, edge2);
            a = dot(edge1, h);

            if (a > -EPS && a < EPS)
                continue;

            f = 1.0f / a;
            s = ray.origin - vertex0;
            u = f * (dot(s, h));

            if (u < 0.0 || u > 1.0)
                continue;

            q = cross(s, edge1);
            v = f * dot(ray.direction, q);

            if (v < 0.0 || u + v > 1.0)
                continue;

            // At this stage we can compute t to find out where the intersection point is on the line.
            float t = f * dot(edge2, q);

            if (t > EPS) // Ray intersection
            {
                if (ray.isInside(t) && t < minDistance)
                {
                    minDistance = t;
                    index = i;
                }
            }
            // else: it means that there is a line intersection but not a ray intersection.
        }

        if (index == -1)
        {
            return false;
        }

        intersection.position    = ray(minDistance);
        intersection.distance    = minDistance;
        intersection.normal      = triangles[index].normal;
        intersection.surfaceType = SurfaceType::Surface;
        intersection.materialID  = triangles[index].materialID;
        intersection.uv          = Vec2f(0, 0);

        return true;
    }

    ScatterEvent sampleSurface(Sampler &sampler) const override
    {
        size_t index = (size_t)sampler.nextDiscrete(triangles.size());
        assert(index < triangles.size());

        float r1 = std::sqrt(sampler.nextFloat());
        float r2 = sampler.nextFloat();

        Vec3f point = triangles[index].v0 * (1 - r1) + triangles[index].v1 * r1 * (1 - r2) + triangles[index].v2 * r1 * r2;

        return ScatterEvent{point, triangles[index].normal, SurfaceType::Surface};
    }

	//获取包围盒
    BoundingBox getBoundingBox() const
    {
        Vec3f min(INF);
        Vec3f max(-INF);

        for (int i = 0; i < (int)triangles.size(); ++i)
        {
            min = minV(min, triangles[i].v0);
            min = minV(min, triangles[i].v1);
            min = minV(min, triangles[i].v2);

            max = maxV(max, triangles[i].v0);
            max = maxV(max, triangles[i].v1);
            max = maxV(max, triangles[i].v2);
        }

        return BoundingBox(min, max);
    }
};

}

#endif //RAYTRACER_GEOMETRY_H
