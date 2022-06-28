#ifndef RAYTRACER_ENUMS_H
#define RAYTRACER_ENUMS_H

//ö�ٱ�������
enum SurfaceType
{
    Surface = 0,
    Volume = (1 << 0)//��1��2��������һλ
};

enum BSDFLobe
{
    Null = 0,
    Diffuse = (1 << 0),//������
    SpecularReflection = (1 << 1),//���淴��
    SpecularTransmission = (1 << 2),//���洫��
    Specular = SpecularReflection | SpecularTransmission//��ѡһ
};

#endif //RAYTRACER_ENUMS_H
