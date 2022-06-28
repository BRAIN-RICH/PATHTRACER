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
    int renderType;//��Ⱦ����
    int minDepth;//��С���
    int maxDepth;//������
    float gamma;//٤�������

    Vec3f backgroundLight;//������

    bool useBox;//ʹ�ð�Χ��
    BoundingBox bb;

    float stepSize;//����
    float densityScale;//
    float gradientFactor;//�ݶ�����

    int octreeLevels;//�˲�������

    TransferFunction transferFunction;

    std::vector<float> brackets;
    std::vector<float> maxOpacity;//���͸����
    std::vector<float> minStepSize;//��С����

    int mask;
};

}

#endif //RAYTRACER_SETTINGS_H
