#include "raytrace.h"

#include "ray.h"
#include "raycast.h"
#include "sampler.h"
#include "settings.h"
#include "scatterevent.h"
#include "vector_type.h"

#include <cassert>
#include <limits>

namespace scg
{

// Currently only works with a single plane, buggy
inline void getBounds(Ray &ray, Settings const& settings)
{
    BBIntersection bbIntersection;
    settings.bb.getIntersection(ray, bbIntersection);

    if (bbIntersection.valid)
    {
        ray.minT = std::max(ray.minT, bbIntersection.nearT);
        ray.maxT = std::min(ray.maxT, bbIntersection.farT);
    }
    else
    {
        ray.minT = std::numeric_limits<float>::max();
        ray.maxT = std::numeric_limits<float>::min();
    }
}

bool getClosestIntersection(
    Scene const& scene,
    Ray const& ray,
    Intersection &closestIntersection,
    Settings const& settings,
    Sampler &sampler)
{
    float minDistance = std::numeric_limits<float>::max();
    int index = -1;

    Intersection intersection;

    if (scene.volume)
    {
        Ray volumeRay = ray;
        if (settings.useBox)
        {
            getBounds(volumeRay, settings);
        }
        volumeRay.origin -= scene.volumePos;

		//三种不同的渲染类型，均为woodcock光线投射
        if ((settings.renderType == 0 && castRayWoodcock(*scene.volume, volumeRay, intersection, settings, sampler)) ||
            (settings.renderType == 1 && castRayWoodcockFast(*scene.volume, volumeRay, intersection, settings, sampler)) ||
            (settings.renderType == 2 && castRayWoodcockFast2(*scene.volume, volumeRay, intersection, settings, sampler)))
        {
            minDistance = intersection.distance;
            index = (int) scene.objects.size();
            closestIntersection = intersection;
            closestIntersection.position += scene.volumePos;
        }
    }

    for (int i = 0; i < (int)scene.objects.size(); ++i)
    {
        if (scene.objects[i]->getIntersection(ray, intersection))
        {
            if (intersection.distance < minDistance)
            {
                minDistance = intersection.distance;
                index = i;
                closestIntersection = intersection;
            }
        }
    }

    if (index == -1)
    {
        return false;
    }

    closestIntersection.objectID = index;

    return true;
}

}