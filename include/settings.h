#ifndef RAYTRACER_SETTINGS_H
#define RAYTRACER_SETTINGS_H

#include "transferfunction.h"
#include "vector_type.h"

#include <utility>
#include <vector>

namespace scg
{

class Settings
{
public:
    int renderType;//渲染类型
    int minDepth;//最小深度
    int maxDepth;//最大深度
    float gamma;//伽马矫正？

    Vec3f backgroundLight;//背景光

    bool useBox;//使用包围盒
    BoundingBox bb;

    float stepSize;//步长
    float densityScale;//
    float gradientFactor;//梯度因素

    int octreeLevels;//八叉树层数

    TransferFunction transferFunction;

    std::vector<float> brackets;
    std::vector<float> maxOpacity;//最大透明度
    std::vector<float> minStepSize;//最小步长

    int mask;
};

}

#endif //RAYTRACER_SETTINGS_H
