#ifndef RAYTRACER_TRANSFERFUNCTION_H
#define RAYTRACER_TRANSFERFUNCTION_H

#include "material.h"
#include "math_utils.h"
#include "vector_type.h"

#include <iostream>
#include <algorithm>
#include <vector>

namespace scg
{
//节点
class Node
{
//private:
public:
    float intensity;//强度
    float opacity;//透明度

    Vec3f colour;//颜色
    std::shared_ptr<Material> material;//智能指针指向材质

    friend class TransferFunction;

public:
    Node(float intensity, float opacity, Vec3f const& colour, std::shared_ptr<Material> const& material):
        intensity(intensity), opacity(opacity), colour(colour), material(material) {};
};

// Nice code design here...
inline bool operator <(float const& intensity, Node const& node)
{
    return intensity < node.intensity;//返回布尔值，判断节点强度
}

class TransferFunction
{
private:
    std::vector<Node> nodes;

public:
    TransferFunction() = default;

    TransferFunction(std::vector<Node> const& nodes):
        nodes(nodes) {};

    inline Vec4f evaluate(float intensity) const
    {
        auto const& upper = std::upper_bound(nodes.begin(), nodes.end(), intensity);
        auto const& lower = upper - 1;

        float dx = (*upper).intensity - (*lower).intensity;
        float dist = (intensity - (*lower).intensity) / dx;

        Vec4f out = Vec4f(
            lerp((*lower).colour.x, (*upper).colour.x, dist),
            lerp((*lower).colour.y, (*upper).colour.y, dist),
            lerp((*lower).colour.z, (*upper).colour.z, dist),
            lerp((*lower).opacity, (*upper).opacity, dist));

        return out;
    }

    inline std::shared_ptr<Material> getMaterial(float intensity, Sampler &sampler) const
    {
        auto const& upper = std::upper_bound(nodes.begin(), nodes.end(), intensity);
        auto const& lower = upper - 1;

        float dx = (*upper).intensity - (*lower).intensity;
        float dist = (intensity - (*lower).intensity) / dx;

        if (sampler.nextFloat() < dist)
        {
            return lower->material;
        }
        else
        {
            return upper->material;
        }
    }
};

}

#endif //RAYTRACER_TRANSFERFUNCTION_H
