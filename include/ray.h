#ifndef RAYTRACER_RAY_H
#define RAYTRACER_RAY_H

#include "vector_type.h"

namespace scg
{

class Ray
{
public:
    Vec3f origin;//��άԭ��
    Vec3f direction;//��ά����
    float minT;//��ʼʱ��
    float maxT;//����ʱ��

    Ray() = default;

	//����ԭ�㣬������ʼʱ�䣬����ʱ�䴴������
    Ray(Vec3f const& origin, Vec3f const& direction, float minT = 0, float maxT = INF):
        origin(origin), direction(normalise(direction)), minT(minT), maxT(maxT) {};

	//���غ������������
    inline Vec3f operator()(float const t) const
    {
        return origin + direction * t;//����ԭ��+����*ʱ�䣬������λ��
    }

	//�жϹ����Ƿ��ں�����
    inline bool isInside(float distance) const
    {
        return minT <= distance && distance <= maxT;
    }
};

}

#endif //RAYTRACER_RAY_H
